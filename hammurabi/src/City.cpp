#include "City.h"

#include <conio.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <random>

#include "utility.h"

namespace
{
	const std::filesystem::path g_SavesPath = "./Saves/";
	const std::string integerInputErrorMessage = "Повелитель, введи число, я не понимаю. ";
}

City::City()
	: m_Round(1),
	m_DeadFromHunger(0),
	m_NewPeople(0),
	m_bPlague(false),
	m_Population(100),
	m_WheatPerAcre(5),
	m_WorkableArea(0),
	m_WheatEatenByRats(0),
	m_WheatReserves(2800),
	m_WheatConsumed(2000),
	m_Area(1000),
	m_AcrePrice(0),
	m_State(GameState::Ongoing)
{
	for (int i = 0; i < 10; i++)
		m_stats[i] = { 0 };

	srand((unsigned)time(NULL));
	if (std::filesystem::exists(g_SavesPath) && !std::filesystem::is_empty(g_SavesPath))
		LoadFromFile();
}


void City::Run()
{
	while (m_State == GameState::Ongoing)
	{
		system("cls");
		BeginRound();
		ProcessInput();
		EndRound();
	}
}


void City::LoadFromFile()
{
	std::cout << "Вы хотели бы продолжить игру с сохранения? Y/N\n";
	
	if (!ProcessOneshotInput())
		return;

	std::cout << "Введите номер или название сохранения.\n";
	std::unordered_map<uint8_t, std::filesystem::path> saveFiles;
	uint32_t saveFileCount = 1;
	for (const auto& saveFile : std::filesystem::directory_iterator(g_SavesPath))
	{
		std::cout << saveFileCount << ": " << saveFile.path() << std::endl;
		saveFiles[saveFileCount] = saveFile.path();
		saveFileCount++;
	}

	std::string input;
	std::filesystem::path chosenFile;
	while (std::cin >> input)
	{
		try
		{
			uint32_t fileNum = std::stoul(input);
			if (fileNum > saveFileCount || fileNum <= 0)
			{
				std::cout << "Введен неправильный номер файла, попробуйте еще раз.\n";
				continue;
			}
			chosenFile = saveFiles[fileNum];
			break;
		}
		catch (const std::invalid_argument& e)
		{
			auto it = std::find_if(saveFiles.begin(), saveFiles.end(),
				[&input](auto& m)
				{
					auto inputPath = std::filesystem::path(input);
					return m.second.filename() == inputPath
						|| m.second.filename().stem() == inputPath;
				});

			if (it == saveFiles.end())
			{
				std::cout << "Такого сохранения нет, попробуйте еще раз.\n";
				continue;
			}
			chosenFile = it->second;
			break;
		}
	}

	std::fstream saveFile{chosenFile, saveFile.in};
	if (!saveFile.is_open())
	{
		std::cout << "Не удалось открыть файл, начинаем новую игру.\n";
		return;
	}

	saveFile >> m_Round;
	saveFile >> m_DeadFromHunger;
	saveFile >> m_NewPeople;
	saveFile >> m_bPlague;
	saveFile >> m_Population;
	
	saveFile >> m_WheatPerAcre;
	saveFile >> m_WorkableArea;
	saveFile >> m_WheatEatenByRats;
	saveFile >> m_WheatReserves;
	saveFile >> m_WheatConsumed;

	saveFile >> m_Area;
	saveFile >> m_AcrePrice;

	for (int i = 0; i < 10; i++)
	{
		saveFile >> m_stats[i].DeadFromHunger;
	}

	return;
}


void City::SaveToFile()
{
	system("cls");
	std::cout << "Введите название файла: ";
	std::string filename;
	std::cin >> filename;
	std::filesystem::path fullPath = g_SavesPath;
	fullPath /= filename;
	
	if (!std::filesystem::exists(g_SavesPath))
		std::filesystem::create_directory(g_SavesPath);

	std::fstream saveFile{fullPath, saveFile.out | saveFile.trunc};

	if (!saveFile.is_open())
	{
		std::cout << "Не удалось сохранить файл, приносим своиз извинения. Скилл ишью.";
		ProcessOneshotInput(false);
		return;
	}

	saveFile << m_Round << std::endl;
	saveFile << m_DeadFromHunger << std::endl;
	saveFile << m_NewPeople << std::endl;
	saveFile << m_bPlague << std::endl;
	saveFile << m_Population << std::endl;
	
	saveFile << m_WheatPerAcre << std::endl;
	saveFile << m_WorkableArea << std::endl;
	saveFile << m_WheatEatenByRats << std::endl;
	saveFile << m_WheatReserves << std::endl;
	saveFile << m_WheatConsumed << std::endl;

	saveFile << m_Area << std::endl;
	saveFile << m_AcrePrice << std::endl;

	for (int i = 0; i < 10; i++)
	{
		saveFile << m_stats[i].DeadFromHunger << std::endl;
	}
}


void City::BeginRound()
{
	std::cout << "Мой повелитель, я хочу поведать тебе о " << m_Round << " годе твоего правления\n";

	if (m_DeadFromHunger)
		std::cout << "	" << m_DeadFromHunger << " человек умерло от голода.\n";

	if (m_NewPeople)
		std::cout << "	" << m_NewPeople << " человек прибыло в наш великий город\n";

	if (m_bPlague)
		std::cout << "	Наш город также постигла чума.\n";
	std::cout << "	Сейчас в городе " << m_Population << " жителей.\n";

	std::cout << "	С каждого акра было собрано " << m_WheatPerAcre
		<< " бушелей, а всего собрано " << m_WorkableArea * m_WheatPerAcre << " бушелей.\n";

	std::cout << "	Крысы съели " << m_WheatEatenByRats << " бушелей пшена!\n";

	std::cout << "	В запасах города осталось " << m_WheatReserves << " бушелей.\n";

	m_AcrePrice = rand() % 10 + 17;
	std::cout << "	В этом году цена акра составляет " << m_AcrePrice << " бушелей пшена.\n";
}


void City::ProcessInput()
{
	std::string input;
	bool boughtLand;
	while (true)
	{
		std::cout << "\nСколько арков земли повелеваешь купить? ";
		int32_t buyAmount = ProcessIntegerInput(input, integerInputErrorMessage);
		if (buyAmount <= 0)
		{
			boughtLand = false;
			break;
		}
		if (buyAmount * m_WheatPerAcre <= m_WheatReserves)
		{
			m_Area += buyAmount;
			m_WheatReserves -= buyAmount * m_AcrePrice;
			boughtLand = true;
			break;
		}
		std::cout << "Правитель, у нас нет столько пшена. У нас "
			<< m_WheatReserves << ", а вы хотите потратить " << buyAmount * m_WheatPerAcre;
	}
	while (!boughtLand)
	{
		std::cout << "Сколько арков земли повелеваешь продать? ";
		int32_t sellAmount = ProcessIntegerInput(input, integerInputErrorMessage);
		if (sellAmount <= 0)
		{
			break;
		}
		if (sellAmount < m_Area)
		{
			m_Area -= sellAmount;
			m_WheatReserves += sellAmount * m_AcrePrice;
			break;
		}
		std::cout << "Правитель, у нас нет столько земель. У нас всего "
			<< m_Area << " акров.";
	}

	while (true)
	{
		std::cout << "Сколько бушелей пшеницы повелеваешь съесть? ";
		int32_t consumptionTarget = ProcessIntegerInput(input, integerInputErrorMessage);
		if (consumptionTarget <= m_WheatReserves)
		{
			m_WheatReserves -= consumptionTarget;
			m_WheatConsumed = consumptionTarget;
			break;
		}
		std::cout << "Правитель, у нас нет столько пшена. У нас всего "
			<< m_WheatReserves << " бушелей.";
	}

	while (true)
	{
		std::cout << "Сколько акров земли повелеваешь засеять? ";
		int32_t plantTarget = ProcessIntegerInput(input, integerInputErrorMessage);
		if (plantTarget <= m_WheatReserves * 2
			&& plantTarget <= m_Population * 10
			&& plantTarget <= m_Area)
		{
			m_WheatReserves -= plantTarget / 2;
			m_WorkableArea = plantTarget;
			break;
		}
		std::cout << "Правитель, помилуй, у нас нет столько пшена, людей или арков. У нас всего "
			<< m_WheatReserves << " бушелей, " << m_Population << " человек и " << m_Area << " арков.";
	}
}


void City::EndRound()
{
	m_WheatPerAcre = rand() % 6 + 1;
	m_WheatReserves += m_WorkableArea * m_WheatPerAcre;

	float eatenCoefficient = float(rand())/RAND_MAX * 0.07;
	m_WheatEatenByRats = uint32_t(eatenCoefficient * m_WheatReserves);
	m_WheatReserves -= m_WheatEatenByRats;

	uint32_t peopleFed = m_WheatConsumed / 20;
	m_DeadFromHunger = m_Population - std::min(m_Population, peopleFed);
	m_stats[m_Round - 1].DeadFromHunger = float(m_DeadFromHunger) / float(m_Population);
	m_Population -= m_DeadFromHunger;
	
	m_NewPeople = std::clamp(int(m_DeadFromHunger) / 2 + (5 - int(m_WheatPerAcre)) * int(m_WheatReserves) / 600 + 1, 0, 50);
	m_Population += m_NewPeople;

	m_bPlague = rand() % 100 <= 15;
	if (m_bPlague)
		m_Population /= 2;

	if (m_stats[m_Round - 1].DeadFromHunger >= 0.45)
	{
		std::cout << "\n\nПо вашей вине погибло слишком много людей! Вы не достойны быть правителем!\n";
		std::cout << "Нажмите любую клавишу, чтобы продолжить.";
		ProcessOneshotInput(false);
		m_State = GameState::Finished;
		return;
	}

	std::cout << "\nОстановить игру и сохранить раунд? Y/N";
	if (ProcessOneshotInput())
	{
		SaveToFile();
		m_State = GameState::Finished;
		return;
	}

	if (m_Round >= 10)
	{
		WinGame();
		m_State = GameState::Finished;
		ProcessOneshotInput(false);
		return;
	}

	m_Round++;
}


void City::WinGame()
{
	float averageDead = 0;
	for (int i = 0; i < 10; i++)
		averageDead += m_stats[i].DeadFromHunger;
	averageDead /= 10;
	int acresPerPerson = m_Area / m_Population;

	system("cls");
	std::cout << "\nПовелитель, ты окончил свое правление!\n";
	if (averageDead > 0.33f && acresPerPerson < 7)
		std::cout << "Из - за вашей некомпетентности в управлении, народ устроил бунт, и изгнал вас их города. Теперь вы вынуждены влачить жалкое существование в изгнании.\n";
	else if (averageDead > 0.1f && acresPerPerson < 9)
		std::cout << "Вы правили железной рукой, подобно Нерону и Ивану Грозному.Народ вздохнул с облегчением, и никто больше не желает видеть вас правителем\n";
	else if (averageDead > 0.03 && acresPerPerson < 10)
		std::cout << "Вы справились вполне неплохо, у вас, конечно, есть недоброжелатели, но многие хотели бы увидеть вас во главе города снова\n";
	else 
		std::cout << "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не справились бы лучше\n\n";
}


