// WaveTableBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../WaveTable/WaveTableGenerator.hpp"
#include <iomanip>

template<size_t TableSize>
void PrintTable(ByteFarm::Tools::Functor functor, std::string heading)
{
	std::cout << heading << " " << TableSize << "\n\n";

	auto wt = ByteFarm::Tools::WaveTableGenerator::Generate<TableSize>(functor);

	for (int i = 0; i < wt.size(); i++)
	{
		std::cout << wt[i];
		std::cout << "f, ";

		if (i % 20 == 0)
			std::cout << "\n";

	}


	std::cout << "\n\n\n";
}

int main()
{
	std::cout << std::setprecision(16);
	

	PrintTable<1024>(ByteFarm::Tools::CommonWaveTableFunctors::Square<8>, "Square 8 Partials");
	PrintTable<1024>(ByteFarm::Tools::CommonWaveTableFunctors::Saw<8>, "Saw 8 Partials");
	PrintTable<1024>(ByteFarm::Tools::CommonWaveTableFunctors::Tri<8>, "Tri 8 Partials");
	PrintTable<1024>(ByteFarm::Tools::CommonWaveTableFunctors::Sine, "Sine");
	
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
