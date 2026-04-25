#include "TuningSys.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

TuningSys::TuningSys()
{
	for (int note = 0; note < 128; ++note)
			notes.push_back(baseFrequency * std::pow(2.0f, (note - midiNoteOffset) / 12.0f));
}

TuningSys::TuningSys(string tuningFile, float baseFreq, int midiNoteOffset) : baseFrequency(baseFreq), midiNoteOffset(midiNoteOffset)
{
	std::ifstream	file(tuningFile);
	if (!file.is_open())
	{
		std::cerr << "Error opening tuning file: " << tuningFile << std::endl;
		return;
	}

	string			line;
	int				lineNum = 0;
	vector<float>	ratios;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '!')
			continue;
		if (lineNum == 0)
			std::cout << "Description: " << line << std::endl;
		else if (lineNum == 1)
		{
			ratios.reserve(std::stoi(line) + 1);
			ratios.push_back(1.0f); // Add the unison ratio for the root note
		}
		else
		{
			if (line.find('/') != string::npos)
			{
				size_t slashPos = line.find('/');
				float numerator = std::stof(line.substr(0, slashPos));
				float denominator = std::stof(line.substr(slashPos + 1));
				ratios.push_back(numerator / denominator);
			}
			else if (line.find('.') != string::npos)
				ratios.push_back(std::pow(2.0f, std::stof(line) / 1200.0f));
			else
				ratios.push_back(std::stof(line));
		}
		lineNum++;
	}

	for (int note = 0; note < 128; ++note)
	{
		int ratioIndex = (note - midiNoteOffset) % ratios.size();
		if (ratioIndex < 0)
			ratioIndex += ratios.size();
		int	cycles = std::floor(static_cast<float>(note - midiNoteOffset) / static_cast<float>(ratios.size()));
		notes.push_back(baseFrequency * ratios[ratioIndex] * std::pow(ratios.back(), cycles));
	}
}

TuningSys::~TuningSys() {}

float	TuningSys::getFrequency(int note) const
{
	if (note < 0 || note >= static_cast<int>(notes.size()))
		return 0.0f; // Return 0 for out-of-range notes
	return notes[note];
}
