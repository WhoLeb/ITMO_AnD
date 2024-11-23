#pragma once

namespace myStl
{
	class MemoryManager
	{
	public:
		MemoryManager();
		~MemoryManager();

		//  No copy no move.
		//? Though might want to be able to move
		MemoryManager(MemoryManager&) = delete;
		MemoryManager& operator=(MemoryManager&) = delete;
		MemoryManager(MemoryManager&&) = delete;
		MemoryManager& operator=(MemoryManager&&) = delete;
	};
}
