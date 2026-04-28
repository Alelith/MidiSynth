#include "Oscilator.hpp"
#include <cmath>

std::vector<float> Oscilator::waveTables[4];

Oscilator::Oscilator(float frequency, float sampleRate, Waveform waveform) : frequency(frequency), sampleRate(sampleRate), phase(0.0), waveform(waveform) {}

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

float	Oscilator::getFrequency() const { return frequency; }

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
