/**
 * @file TuningSys.cpp
 * @brief Implementation of the microtonal scaling architecture
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#include "TuningSys.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

/**
 * @brief Constructs a default TuningSys with standard equal temperament
 * 
 * @details Generates a full 128-element MIDI array mapped using the
 * traditional 12-Tone Equal Temperament mathematical sequence based
 * around the default base frequency.
 * 
 * @ingroup tuning_module
 */
TuningSys::TuningSys()
{
	for (int note = 0; note < 128; ++note)
			notes.push_back(baseFrequency * std::pow(2.0f, (note - midiNoteOffset) / 12.0f));
}

/**
 * @brief Constructs a Custom TuningSys derived from a Scala configuration
 * 
 * @details Reads and parses a standard .scl format file determining
 * relative mathematical ratios for notes in relation to the defined 
 * base frequency and corresponding MIDI anchor. It then extrapolates
 * valid pitch lengths mapping every defined cycle repetition across
 * the whole MIDI range.
 * 
 * @ingroup tuning_module
 * 
 * @param[in] tuningFile Relative or absolute path to a .scl Scala file
 * @param[in] baseFreq The absolute hertz value for the tuning origin
 * @param[in] midiNoteOffset The standard MIDI index matching the origin 
 */
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

/**
 * @brief Default destructor for the TuningSys object
 * 
 * @details Implicitly releases dynamically allocated table memory vectors
 * containing calculated frequency bounds.
 * 
 * @ingroup tuning_module
 */
TuningSys::~TuningSys() {}

/**
 * @brief Retrieves the pre-calculated target frequency for a MIDI event
 * 
 * @details Safely bounds-checks the incoming key index map and returns
 * the pre-populated static float representation for the internal oscillator.
 * 
 * @ingroup tuning_module
 * 
 * @param[in] note The numerical standard MIDI note key query (0 - 127)
 * @return The target ideal floating-point pitch value in Hz
 * @retval 0.0f Query falls out of the valid lookup array bound
 */
float	TuningSys::getFrequency(int note) const
{
	if (note < 0 || note >= static_cast<int>(notes.size()))
		return 0.0f; // Return 0 for out-of-range notes
	return notes[note];
}
