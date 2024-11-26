#pragma once

#include <string>

enum class GameState : uint8_t
{
	Ongoing,
	Finished,
};

class City
{
	struct Statistics
	{
		float DeadFromHunger;
	};

public:
	City();
	City(City&) = delete;
	City& operator=(City&) = delete;
	City(City&&) = delete;
	City& operator=(City&&) = delete;

	void Run();

private:
	void LoadFromFile();
	void SaveToFile();

	void BeginRound();
	void ProcessInput();
	void EndRound();
	void WinGame();

private:
	uint32_t m_Round;

	uint32_t m_DeadFromHunger;
	uint32_t m_NewPeople;
	bool m_bPlague;
	uint32_t m_Population;
	
	uint32_t m_WheatPerAcre;
	uint32_t m_WorkableArea;
	uint32_t m_WheatEatenByRats;
	uint32_t m_WheatReserves;
	uint32_t m_WheatConsumed;

	uint32_t m_Area;
	uint32_t m_AcrePrice;

	GameState m_State;

	Statistics m_stats[10];
};

