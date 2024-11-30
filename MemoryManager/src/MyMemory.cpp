#include "MyMemory.h"

#ifndef NDEBUG
	#include <iostream>

namespace
{
	void printHLine()
	{
		std::cout << "\n";
		for (int i = 0; i < 60; i++)
			std::cout << "-";
		std::cout << "\n";
	}
}
#endif // !NDEBUG


namespace myStl
{
#pragma region MemoryManager public

	 MemoryManager::~MemoryManager()
	{
		assert(!m_bIsInitialized && "MemoryManager must be destroyed befor destruction");

		if (m_bIsInitialized)
			destroy();
	}

	void MemoryManager::init()
	{
		assert(!m_bIsInitialized && "MemoryManager is already initilized");

		if (m_bIsInitialized)
			return;

		const size_t blockSizes[NUM_BLOCK_SIZES] = { 16, 32, 64, 128, 256, 512 };
		for (int i = 0; i < NUM_BLOCK_SIZES; i++)
			fsAllocators[i].init(blockSizes[i], (UINT8)i);

		freeListAllocator.init(1024 * 4);
		m_bIsInitialized = true;
	}

	void MemoryManager::destroy()
	{
		assert(m_bIsInitialized && "MemoryManager is not initialized");
		if (!m_bIsInitialized)
			return;

		OSAllocation* current = m_OSAllocations;
		while (current)
		{
			OSAllocation* toDelete = current;
			current = current->next;
			void* regionMemory = (void*)((char*)toDelete - sizeof(RegionHeader));
			VirtualFree(regionMemory, 0, MEM_RELEASE);
		}
		m_OSAllocations = nullptr;

		for (auto& iter : fsAllocators)
			iter.destroy();

		freeListAllocator.destroy();

#ifndef NDEBUG
		// Check for memory leaks
		for (int i = 0; i < NUM_BLOCK_SIZES; ++i)
		{
			assert(fsAllocators[i].isEmpty() && "Memory leak detected in FSAllocator");
		}
		assert(freeListAllocator.isEmpty() && "Memory leak detected in FreeListAllocator");
		if (m_OSAllocations != nullptr)
		{
			std::cerr << "Memory leak detected in OS Allocations" << std::endl;
			dumpBlocks();
		}
#endif

		// No need to delete allocators since they are stack-allocated

		m_bIsInitialized = false;
	}

	void* MemoryManager::alloc(size_t size)
	{
		assert(m_bIsInitialized && "MemoryManager is not initialized");
		if (!m_bIsInitialized)
			return nullptr;

		void* ptr = nullptr;
		if (size <= 512)
		{
			size_t index = getBlockSizeIndex(size);
			if (index < NUM_BLOCK_SIZES)
			{
				ptr = fsAllocators[index].alloc();
			}
			else
			{
				assert(false && "Invalid block size index");
			}
		}
		else if (size <= 10 * 1024 * 1024) // 10 MB
		{
			ptr = freeListAllocator.alloc(size);
		}
		else
		{
			// Allocate directly from OS
			size_t totalSize = align(size) + sizeof(RegionHeader) + sizeof(OSAllocation);
			void* regionMemory = VirtualAlloc(nullptr, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (!regionMemory)
				return nullptr;

			// Place the RegionHeader at the beginning
			RegionHeader* regionHeader = (RegionHeader*)regionMemory;
			regionHeader->allocatorType = (UINT8)AllocatorType::OS;
			regionHeader->allocatorIndex = 0;

			// Usable memory starts after RegionHeader
			// OSAllocation struct is placed immediately after RegionHeader
			OSAllocation* osAlloc = (OSAllocation*)((char*)regionMemory + sizeof(RegionHeader));
			osAlloc->size = size;
			osAlloc->address = (void*)((char*)osAlloc + sizeof(OSAllocation));
			osAlloc->next = m_OSAllocations;
			m_OSAllocations = osAlloc;

			// Usable memory starts after OSAllocation
			void* usableMemory = osAlloc->address;

			ptr = usableMemory;
		}

		return ptr;
	}

	void MemoryManager::free(void* ptr)
	{
		assert(m_bIsInitialized && "MemoryManager is not initialized");
		if (!ptr)
			return;

		// Attempt to get the RegionHeader
		RegionHeader* regionHeader = getRegionHeader(ptr);
		if (!regionHeader)
		{
			assert(false && "Invalid pointer passed to free");
			return;
		}

		switch ((AllocatorType)regionHeader->allocatorType)
		{
		case AllocatorType::FixedSize:
		{
			UINT8 allocatorIndex = regionHeader->allocatorIndex;
			if (allocatorIndex < NUM_BLOCK_SIZES)
			{
				fsAllocators[allocatorIndex].free(ptr);
			}
			else
			{
				assert(false && "Invalid FSAllocator index");
			}
			break;
		}
		case AllocatorType::FreeList:
		{
			freeListAllocator.free(ptr);
			break;
		}
		case AllocatorType::OS:
		{
			OSAllocation** current = &m_OSAllocations;
			OSAllocation* osAllocToFree = nullptr;
			while (*current)
			{
				if ((*current)->address == ptr)
				{
					osAllocToFree = *current;
					*current = (*current)->next;
					break;
				}
				current = &((*current)->next);
			}

			if (osAllocToFree)
			{
				// Free via OS
				void* regionMemory = (void*)regionHeader;
				if(!VirtualFree(regionMemory, 0, MEM_RELEASE))
					assert(false);
			}
			else
			{
				assert(false && "OS allocation not found in list");
			}
			break;
		}
		default:
			assert(false && "Unknown allocator type");
			break;
		}
	}

#ifndef NDEBUG

	void MemoryManager::dumpStat() const
	{
		assert(m_bIsInitialized && "MemoryManager is not initialized");
		printHLine();
		std::cout << "MemoryManager Statistics:" << std::endl;
		for (int i = 0; i < NUM_BLOCK_SIZES; ++i)
		{
			printHLine();
			std::cout << "FSAllocator[" << i << "]:" << std::endl;
			fsAllocators[i].dumpStat();
			std::cout << std::endl;
		}

		printHLine();
		std::cout << "FreeListAllocator:" << std::endl;
		freeListAllocator.dumpStat();
		std::cout << std::endl;

		// OS Allocations statistics
		printHLine();
		size_t totalOSAllocations = 0;
		size_t totalOSMemory = 0;
		OSAllocation* current = m_OSAllocations;
		while (current)
		{
			totalOSAllocations++;
			totalOSMemory += current->size;
			current = current->next;
		}
		std::cout << "OS Allocations:" << std::endl;
		std::cout << "Total OS Allocations: " << totalOSAllocations << std::endl;
		std::cout << "Total OS Memory: " << totalOSMemory << " bytes" << std::endl;
	}

	void MemoryManager::dumpBlocks() const
	{
		assert(m_bIsInitialized && "MemoryManager is not initialized");
		std::cout << "MemoryManager Blocks:" << std::endl;
		for (int i = 0; i < NUM_BLOCK_SIZES; ++i)
		{
			std::cout << "FSAllocator[" << i << "] Blocks:" << std::endl;
			fsAllocators[i].dumpBlocks();
			std::cout << std::endl;
		}
		std::cout << "FreeListAllocator Blocks:" << std::endl;
		freeListAllocator.dumpBlocks();
		std::cout << std::endl;
		// OS Allocations blocks
		OSAllocation* current = m_OSAllocations;
		std::cout << "OS Allocations Blocks:" << std::endl;
		while (current)
		{
			std::cout << "OS Allocation at " << current->address
				<< " | Size: " << current->size << " bytes" << std::endl;
			current = current->next;
		}
	}

#endif // !NDEBUG

#pragma endregion

#pragma region MemoryManager private

	 size_t MemoryManager::getBlockSizeIndex(size_t size) const
	{
		for (int i = 16, j = 0; i <= 512; i *= 2, j++)
			if (size <= i)
				return j;
		return NUM_BLOCK_SIZES;
	}

	MemoryManager::RegionHeader* MemoryManager::getRegionHeader(void* p)
	{
		if (!p)
			return nullptr;

		// Check if the pointer belongs to OS memory
		OSAllocation* currentOS = m_OSAllocations;
		while (currentOS)
		{
			if(currentOS + 1 == p)
				return (RegionHeader*)((char*)p - sizeof(RegionHeader) - sizeof(OSAllocation));
			currentOS = currentOS->next;
		}

		// Check if the pointer belongs to FixedSize allocator
		for (int i = 0; i < NUM_BLOCK_SIZES; i++)
		{
			if(fsAllocators[i].find(p))
				return ((FSBlock*)((char*)p - sizeof(FSBlock)))->regionHeader;
		}

		// Check if the pointer belongs to FreeList allocator
		if(freeListAllocator.find(p))
			return ((FLBlock*)((char*)p - sizeof(FLBlock)))->regionHeader;

		// Pointer does not belong to any known allocator
		return nullptr;
	}

#pragma endregion

#pragma region FSA public

	 void FSAllocator::init(size_t blockSize, UINT8 allocatorIndex)
	{
		m_BlockSize = align(blockSize);
		m_AllocatorIndex = allocatorIndex;
		size_t regionSize = calculateRegionSize(m_BlockSize);
		m_MemoryRegions.init(regionSize);
		m_Head = nullptr;
		m_AllocatedBlockCount = 0;
	}

	 void* FSAllocator::alloc()
	{
		if (!m_Head)
		{
			if (!acquireNewRegion())
				return nullptr;
		}

		Block* block = m_Head;
		m_Head = m_Head->next;

#ifndef NDEBUG
		block->used = true;
#endif

		RegionHeader* regionHeader = block->regionHeader;
		regionHeader->allocatedBlocks++;

		m_AllocatedBlockCount++;

		return (void*)((char*)block + sizeof(Block));
	}

	 void FSAllocator::free(void* ptr)
	{
		if (!ptr)
			return;

		Block* block = (Block*)((char*)ptr - sizeof(Block));

#ifndef NDEBUG
		if (!block->used)
		{
			std::cerr << "\nError: Attempt to free a block that is already free.\n" << std::endl;
			assert(false && "Double-free detected in FSAllocator::free");
			return;
		}
		block->used = false;
#endif

		RegionHeader* regionHeader = block->regionHeader;

		assert(regionHeader->allocatorType == (UINT8)AllocatorType::FixedSize);
		assert(regionHeader->allocatorIndex == m_AllocatorIndex);

		regionHeader->allocatedBlocks--;
		m_AllocatedBlockCount--;

		insertBlockIntoFreeList(block);

		if (regionHeader->allocatedBlocks == 0)
		{
			removeRegionBlocksFromFreeList(block->regionHeader);

			void* usableMemory = (void*)((char*)regionHeader + sizeof(RegionHeader));
			m_MemoryRegions.releaseRegion(usableMemory);
		}
	}

#ifndef NDEBUG

	// Dump statistics about the allocator

	 void FSAllocator::dumpStat() const
	{
		std::cout << "FSAllocator[" << (int)m_AllocatorIndex << "] Statistics:" << std::endl;
		size_t totalBlocks = 0;
		size_t allocatedBlocks = 0;
		size_t freeBlocks = 0;
		size_t totalMemory = 0;
		size_t allocatedMemory = 0;
		size_t freeMemory = 0;

		size_t blockTotalSize = sizeof(Block) + m_BlockSize;

		MemoryRegions::Region* region = m_MemoryRegions.getHead();
		while (region)
		{
			// Usable memory starts after RegionHeader
			char* regionHeaderMemory = (char*)region + sizeof(MemoryRegions::Region);
			RegionHeader* regionHeader = (RegionHeader*)regionHeaderMemory;
			char* blockMemoryStart = (char*)regionHeaderMemory + sizeof(RegionHeader);
			size_t numBlocks = m_MemoryRegions.getRegionSize() / blockTotalSize;

			for (size_t i = 0; i < numBlocks; ++i)
			{
				Block* block = (Block*)(blockMemoryStart + i * blockTotalSize);
				totalBlocks++;
				totalMemory += m_BlockSize;

				if (block->used)
				{
					allocatedBlocks++;
					allocatedMemory += m_BlockSize;
				}
				else
				{
					freeBlocks++;
					freeMemory += m_BlockSize;
				}
			}

			region = region->next;
		}

		std::cout << "Total Blocks: " << totalBlocks << std::endl;
		std::cout << "Allocated Blocks: " << allocatedBlocks << std::endl;
		std::cout << "Free Blocks: " << freeBlocks << std::endl;
		std::cout << "Total Memory: " << totalMemory << " bytes" << std::endl;
		std::cout << "Allocated Memory: " << allocatedMemory << " bytes" << std::endl;
		std::cout << "Free Memory: " << freeMemory << " bytes" << std::endl;
	}

	// Dump the list of blocks

	 void FSAllocator::dumpBlocks() const
	{
		std::cout << "FSAllocator[" << (int)m_AllocatorIndex << "] Blocks:" << std::endl;

		size_t blockTotalSize = sizeof(Block) + m_BlockSize;

		MemoryRegions::Region* region = m_MemoryRegions.getHead();
		while (region)
		{
			// Usable memory starts after RegionHeader
			char* regionHeaderMemory = (char*)region + sizeof(MemoryRegions::Region);
			RegionHeader* regionHeader = (RegionHeader*)regionHeaderMemory;
			char* blockMemoryStart = (char*)regionHeaderMemory + sizeof(RegionHeader);
			size_t numBlocks = m_MemoryRegions.getRegionSize() / blockTotalSize;

			for (size_t i = 0; i < numBlocks; ++i)
			{
				Block* block = (Block*)(blockMemoryStart + i * blockTotalSize);
				std::cout << "Block at " << (void*)block
					<< " | Used: " << (block->used ? "Yes" : "No") << std::endl;
			}

			region = region->next;
		}
	}


#endif // !NDEBUG


#pragma endregion

#pragma region FSA private

	size_t FSAllocator::calculateRegionSize(size_t blockSize) const
	{
		size_t blockTotalSize = sizeof(Block) + blockSize;
		size_t numBlocks = (1024 * 10) / blockTotalSize;
		return align(numBlocks * blockTotalSize);
	}

	bool FSAllocator::acquireNewRegion()
	{
		void* regionMemory = m_MemoryRegions.acquireRegion();
		if (!regionMemory)
			return false;

		// Place the region header at the beginning
		RegionHeader* regionHeader = (RegionHeader*)((char*)regionMemory - sizeof(RegionHeader));
		regionHeader->allocatedBlocks = 0;
		regionHeader->allocatorType = (UINT8)AllocatorType::FixedSize;
		regionHeader->allocatorIndex = m_AllocatorIndex;

		size_t blockTotalSize = sizeof(Block) + m_BlockSize;
		size_t numBlocks = m_MemoryRegions.getRegionSize() / blockTotalSize;
		char* blockAddress = (char*)regionMemory;

		for (size_t i = 0; i < numBlocks; ++i)
		{
			Block* block = (Block*)blockAddress;
			block->regionHeader = regionHeader;
#ifndef NDEBUG
			block->used = false;
#endif
			insertBlockIntoFreeList(block);

			blockAddress += blockTotalSize;
		}

		return true;
	}

	void FSAllocator::insertBlockIntoFreeList(Block* block)
	{

		block->next = m_Head;
		m_Head = block;
		//if (!m_Head || block < m_Head)
		//{
		//	block->next = m_Head;
		//	m_Head = block;
		//}
		//else
		//{
		//	Block* current = m_Head;
		//	while (current->next && current->next < block)
		//	{
		//		current = current->next;
		//	}

		//	block->next = current->next;
		//	current->next = block;
		//}
	}

	void FSAllocator::removeRegionBlocksFromFreeList(RegionHeader* regionHeader)
	{
		Block** current = &m_Head;
		while (*current)
		{
			if ((*current)->regionHeader == regionHeader)
			{
				*current = (*current)->next;
			}
			else
			{
				current = &(*current)->next;
			}
		}
	}
#pragma endregion

#pragma region Free list public

	void* FreeListAllocator::alloc(size_t size)
	{
		size = align(size);

		size_t totalSize = size + sizeof(Block);

		// Search the free list for a suitable block
		Block* block = findFreeBlock(totalSize);
		if (block)
		{
			splitBlock(block, totalSize);
			block->used = true;
			return (void*)((char*)block + sizeof(Block));
		}

		// No suitable block found; acquire a new region
		if (!acquireNewRegion())
			return nullptr;

		// Try allocation again
		block = findFreeBlock(totalSize);
		if (block)
		{
			splitBlock(block, totalSize);
			block->used = true;
			return (void*)((char*)block + sizeof(Block));
		}

		// Allocation failed
		return nullptr;
	}

	void FreeListAllocator::free(void* ptr)
	{
		if (!ptr)
			return;
		Block* block = (Block*)((char*)ptr - sizeof(Block));
		RegionHeader* regionHeader = block->regionHeader;

		assert(regionHeader->allocatorType == (UINT8)AllocatorType::FreeList);

		block->used = false;
		coalesce(block);
	}

	bool FreeListAllocator::isEmpty() const
	{
		Block* current = m_Head;
		while (current)
		{
			if (current->used)
				return false;
			current = current->next;
		}
		return true;
	}

#ifndef NDEBUG
	// Dump statistics about the allocator
	void FreeListAllocator::dumpStat() const
	{
		std::cout << "FreeListAllocator Statistics:" << std::endl;
		size_t totalBlocks = 0;
		size_t allocatedBlocks = 0;
		size_t freeBlocks = 0;
		size_t totalMemory = 0;
		size_t allocatedMemory = 0;
		size_t freeMemory = 0;

		Block* current = m_Head;
		while (current)
		{
			totalBlocks++;
			totalMemory += current->size;
			if (current->used)
			{
				allocatedBlocks++;
				allocatedMemory += current->size;
			}
			else
			{
				freeBlocks++;
				freeMemory += current->size;
			}
			current = current->next;
		}

		std::cout << "Total Blocks: " << totalBlocks << std::endl;
		std::cout << "Allocated Blocks: " << allocatedBlocks << std::endl;
		std::cout << "Free Blocks: " << freeBlocks << std::endl;
		std::cout << "Total Memory: " << totalMemory << " bytes" << std::endl;
		std::cout << "Allocated Memory: " << allocatedMemory << " bytes" << std::endl;
		std::cout << "Free Memory: " << freeMemory << " bytes" << std::endl;
	}

	// Dump the list of blocks

	void FreeListAllocator::dumpBlocks() const
	{
		std::cout << "FreeListAllocator Blocks:" << std::endl;
		Block* current = m_Head;
		while (current)
		{
			std::cout << "Block at " << current
				<< " | Size: " << current->size
				<< " | Used: " << (current->used ? "Yes" : "No")
				<< std::endl;
			current = current->next;
		}
	}

#endif

#pragma endregion

#pragma region Free list private

	FreeListAllocator::Block* FreeListAllocator::findFreeBlock(size_t size)
	{
		Block* current = m_Head;
		while (current)
		{
			if (!current->used && current->size >= size)
				return current;
			current = current->next;
		}
		return nullptr;
	}

	void FreeListAllocator::splitBlock(Block* block, size_t size)
	{
		size = align(size);
		if (block->size >= size + sizeof(Block) + MIN_BLOCK_SIZE)
		{
			Block* newBlock = (Block*)((char*)block + size);
			newBlock->size = block->size - (size + sizeof(Block));
			newBlock->used = false;
			newBlock->next = block->next;
			newBlock->previous = block;
			newBlock->regionHeader = block->regionHeader;

			if (newBlock->next)
				newBlock->next->previous = newBlock;

			block->size = size;
			block->next = newBlock;
		}
	}

	void FreeListAllocator::coalesce(Block* block)
	{
		if (block->previous && !block->previous->used && block->previous->regionHeader == block->regionHeader)
		{
			block->previous->size += sizeof(Block) + block->size;
			block->previous->next = block->next;
			if (block->next)
				block->next->previous = block->previous;
			block = block->previous;
		}

		if (block->next && !block->next->used && block->next->regionHeader == block->regionHeader)
		{
			block->size += sizeof(Block) + block->next->size;
			block->next = block->next->next;
			if (block->next)
				block->next->previous = block;
		}

		char* usableMemoryStart = (char*)block->regionHeader + align(sizeof(RegionHeader));
		char* blockStart = (char*)block;
		size_t regionUsableSize = m_MemoryRegions.getRegionSize();

		if (blockStart == usableMemoryStart && block->size == regionUsableSize - sizeof(Block))
		{
			if (block->previous)
				block->previous->next = block->next;
			else
				m_Head = block->next;
			if (block->next)
				block->next->previous = block->previous;

			void* usableMemory = (void*)usableMemoryStart;
			m_MemoryRegions.releaseRegion(usableMemory);
		}
	}

	bool FreeListAllocator::acquireNewRegion()
	{
		void* usableMemory = m_MemoryRegions.acquireRegion();
		if (!usableMemory)
			return false;

		RegionHeader* regionHeader = (RegionHeader*)((char*)usableMemory - sizeof(RegionHeader));
		regionHeader->allocatorType = (UINT8)AllocatorType::FreeList;
		regionHeader->allocatorIndex = 0;

		Block* regionBlock = (Block*)usableMemory;
		regionBlock->size = m_MemoryRegions.getRegionSize() - sizeof(Block);
		regionBlock->used = false;
		regionBlock->next = nullptr;
		regionBlock->previous = nullptr;
		regionBlock->regionHeader = regionHeader;

		addBlockToFreeList(regionBlock);
		return true;
	}

	void FreeListAllocator::addBlockToFreeList(Block* block)
	{
		if (!m_Head)
		{
			m_Head = block;
			block->next = nullptr;
			block->previous = nullptr;
		}
		else
		{
			block->next = m_Head;
			block->previous = nullptr;
			m_Head->previous = block;
			m_Head = block;
		}
	}

#pragma endregion

#pragma region Memory regions public

	void MemoryRegions::destroy()
	{
		Region* current = m_Head;
		while (current)
		{
			releaseRegion(current);
			current = current->next;
		}
	}

	void* MemoryRegions::acquireRegion()
	{
		size_t totalSize = sizeof(Region) + sizeof(RegionHeader) + m_RegionSize;
		Region* newRegion = (Region*)VirtualAlloc(nullptr, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (!newRegion)
			return nullptr;

		newRegion->next = m_Head;
		m_Head = newRegion;

		void* regionHeaderMemory = (void*)((char*)newRegion + sizeof(Region));

		return (void*)((char*)regionHeaderMemory + sizeof(RegionHeader));
	}

	void MemoryRegions::releaseRegion(void* regionMemory)
	{
		if (!regionMemory) return;

		Region* region = (Region*)((char*)regionMemory - sizeof(RegionHeader) - sizeof(Region));

		if (m_Head == region)
			m_Head = region->next;
		else
		{
			Region* prev = m_Head;
			while (prev && prev->next != region)
				prev = prev->next;

			if (prev)
				prev->next = region->next;
		}

		VirtualFree(region, 0, MEM_RELEASE);
	}

#pragma endregion

}
