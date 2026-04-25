#include "AudioEngine.hpp"
#include <chrono>
#include <thread>


int main()
{
	AudioEngine au;
	au.setModulationIndex(250);
	au.setModulationRatio(1.5);
	std::cin.get();
}