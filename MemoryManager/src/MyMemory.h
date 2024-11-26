#pragma once

#include <cassert>
#include "Windows.h"

/*
	So, the idea is to have an FSA for sizes 16, 32, 64, 128, 256 and 512 bytes.
	Then from 512 to 10 kB a coalesce allocator should take over.
	Finaly over that should go straight to OS.

	I think I should do separate classes for each allocation policy.
*/

namespace myStl
{
	inline size_t align(size_t n)
	{
		return (n + sizeof(intptr_t) - 1) & ~(sizeof(intptr_t) - 1);
	}

	enum class AllocatorType : UINT8
	{
		FixedSize = 0,
		FreeList = 1,
		OS = 2
	};

	// This is a list which simply holds pointers to large memory regions 
	// which should fit both for FSA and FLA
	class MemoryRegions
	{
	public: 
		struct Region
		{
			Region* next;
		};

		MemoryRegions() : m_Head(nullptr), m_RegionSize(0) {}

		void init(size_t regionSize)
		{
			m_RegionSize = align(regionSize);
			m_Head = nullptr;
		}
		void destroy();

		void* acquireRegion();

		void releaseRegion(void* regionMemory);
		
		size_t getRegionSize() const { return m_RegionSize; }

		Region* getHead() const { return m_Head; }
	private:

		Region* m_Head;
		size_t m_RegionSize;

		struct RegionHeader
		{
			UINT8 allocatorType;
			UINT8 allocatorIndex;
			UINT16 padding;
			size_t padding2;
		};
	};

	// The fsa allocator.
	class FSAllocator
	{
	public:
		FSAllocator()
			: m_BlockSize(0),
			  m_MemoryRegions(),
			  m_Head(nullptr),
			  m_AllocatorIndex(0),
			  m_AllocatedBlockCount(0)
		{}

		void init(size_t blockSize, UINT8 allocatorIndex);
		void destroy()
		{
			m_MemoryRegions.destroy();
		}

		void* alloc();

		void free(void* ptr);

		bool isEmpty() const { return m_AllocatedBlockCount == 0; }

#ifndef NDEBUG
		// Dump statistics about the allocator
		void dumpStat() const;

		// Dump the list of blocks
		void dumpBlocks() const;

#endif // !NDEBUG

	private:

		struct RegionHeader
		{
			UINT8 allocatorType;
			UINT8 allocatorIndex;
			UINT16 padding;
			size_t allocatedBlocks;
		};

		struct Block
		{
			Block* next;
			RegionHeader* regionHeader;
#ifndef NDEBUG
			bool used;
#endif
		};

		size_t calculateRegionSize(size_t blockSize) const;

		bool acquireNewRegion();

		void insertBlockIntoFreeList(Block* block);

		void removeRegionBlocksFromFreeList(RegionHeader* regionHeader);

	private:
		size_t m_BlockSize;
		Block* m_Head;
		MemoryRegions m_MemoryRegions;
		UINT8 m_AllocatorIndex;
		size_t m_AllocatedBlockCount;
	};

	// The coalesce allocator
	class FreeListAllocator
	{
	public:

		FreeListAllocator() : m_MemoryRegions(), m_Head(nullptr) {}

		void init(size_t regionSize = 1024 * 1024 * 32)
		{
			m_MemoryRegions.init(regionSize);
			m_Head = nullptr;
		}
		void destroy()
		{
			m_MemoryRegions.destroy();
		}

		void* alloc(size_t size);

		void free(void* ptr);

		bool isEmpty() const;

#ifndef NDEBUG
		// Dump statistics about the allocator
		void dumpStat() const;

		// Dump the list of blocks
		void dumpBlocks() const;
#endif

	private:
		struct RegionHeader
		{
			UINT8 allocatorType;
			UINT8 allocatorIndex;
			UINT16 padding;
			UINT32 padding2;
		};

		struct Block
		{
			size_t size;
			bool used;
			Block* next;
			Block* previous;
			RegionHeader* regionHeader;
		};

		Block* findFreeBlock(size_t size);

		void splitBlock(Block* block, size_t size);

		void coalesce(Block* block);

		bool acquireNewRegion();

		void addBlockToFreeList(Block* block);

	private:
		MemoryRegions m_MemoryRegions;
		Block* m_Head{ nullptr };

		static const size_t MIN_BLOCK_SIZE = 768;
	};

	class MemoryManager
	{
	public:
		MemoryManager() : m_bIsInitialized(false), m_OSAllocations(nullptr) {}
		~MemoryManager();

		void init();

		void destroy();

		void* alloc(size_t size);

		void free(void* ptr);

#ifndef NDEBUG

		void dumpStat() const;

		void dumpBlocks() const;

#endif // !NDEBUG

	private:
		struct RegionHeader
		{
			UINT8 allocatorType;
			UINT8 allocatorIndex;
			UINT16 padding;
			UINT32 padding2;
		};

		struct FSBlock
		{
			void* next;
			RegionHeader* regionHeader;
#ifndef NDEBUG
			bool used;
#endif
		};

		struct FLBlock
		{
			size_t size;
			bool used;
			void* next;
			void* previous;
			RegionHeader* regionHeader;
		};

		struct OSAllocation
		{
			OSAllocation* next;
			size_t size;
			void* address;
		};

	private:

		size_t getBlockSizeIndex(size_t size) const;

		RegionHeader* getRegionHeader(void* p);

	private:
		
		static const size_t NUM_BLOCK_SIZES = 6;
		FSAllocator fsAllocators[NUM_BLOCK_SIZES];
		FreeListAllocator freeListAllocator;
		OSAllocation* m_OSAllocations;
		bool m_bIsInitialized{ false };
	};


}
