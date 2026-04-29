/**
 * @file Oscilator.cpp
 * @brief Implementation of the waveform tables and phase generation
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#include "Oscilator.hpp"
#include <cmath>

std::vector<float> Oscilator::waveTables[4];

/**
 * @brief Constructs a fresh oscillator instance and sets initial phase
 * 
 * @details Synchronizes the primary target frequency and wave shape
 * properties along with the global sample rate for stepping calculations.
 * 
 * @ingroup oscilator_module
 * 
 * @param[in] frequency Fundamental operational pitch mapping in Hz
 * @param[in] sampleRate Frequency of the output context system
 * @param[in] waveform The shape selection for base generation calculations
 */
Oscilator::Oscilator(float frequency, float sampleRate, Waveform waveform) : frequency(frequency), sampleRate(sampleRate), phase(0.0), waveform(waveform) {}

/**
 * @brief Requests the next consecutive sample calculation output
 * 
 * @details Implements lookup table interpolation to retrieve accurate
 * frames based on phase, stepping the internal phase along by combined
 * internal fundamental frequency and external dynamic FM modulation factor.
 * 
 * @ingroup oscilator_module
 * 
 * @param[in] modulation Additive frequency component value (FM input)
 * @return Next scalar float sample generated from currently chosen WaveTable
 */
float	Oscilator::nextSample(float modulation)
{
	const std::vector<float>& table = waveTables[static_cast<int>(waveform)];
	float	tableIndex = (phase / (2.0 * M_PI)) * table.size();
	int		index0 = static_cast<int>(tableIndex) % table.size();
	int		index1 = (index0 + 1) % table.size();
	float	frac = tableIndex - std::floor(tableIndex);

	float	sample = (1.0f - frac) * table[index0] + frac * table[index1];

	phase += (2.0 * M_PI * (frequency + modulation)) / sampleRate;
	while (phase >= 2.0 * M_PI)
		phase -= 2.0 * M_PI;
	while (phase < 0.0)
		phase += 2.0 * M_PI;
	
	return sample;
}

/**
 * @brief Retrieves the currently set unmodulated base frequency
 * 
 * @details Reads back the original configured tone pitch in Hz
 * configured at Note execution.
 * 
 * @ingroup oscilator_module
 * 
 * @return Target pitch frequency
 */
float	Oscilator::getFrequency() const { return frequency; }

/**
 * @brief Populates memory buffers with math-computed waveform cycle frames
 * 
 * @details Before runtime synthesis, this function generates integer-sized lookup
 * tables spanning one complete phase representing each waveform statically.
 * This guarantees execution performance in the tight audio callback loop.
 * 
 * @ingroup oscilator_module
 * 
 * @param[in] tableSize Sample array length forming a phase lookup loop
 */
void	Oscilator::initWaveTables(int tableSize)
{
	waveTables[static_cast<int>(Waveform::SINE)].resize(tableSize);
	waveTables[static_cast<int>(Waveform::SQUARE)].resize(tableSize);
	waveTables[static_cast<int>(Waveform::SAWTOOTH)].resize(tableSize);
	waveTables[static_cast<int>(Waveform::TRIANGLE)].resize(tableSize);

	for (int i = 0; i < tableSize; ++i)
	{
		float	phase = (static_cast<float>(i) / tableSize) * 2.0f * M_PI;
		waveTables[static_cast<int>(Waveform::SINE)][i] = sin(phase);
		waveTables[static_cast<int>(Waveform::SQUARE)][i] = (phase < M_PI) ? 1.0f : -1.0f;
		float	t = static_cast<float>(i) / tableSize;
		waveTables[static_cast<int>(Waveform::SAWTOOTH)][i] = (2.0f * t) - 1.0f;
		waveTables[static_cast<int>(Waveform::TRIANGLE)][i] = (phase < M_PI) ? (4.0f * t) - 1.0f : 3.0f - (4.0f * t);
	}
}
