#include "Oscilator.hpp"
#include <cmath>

std::map<Waveform, std::vector<float>> Oscilator::waveTables;

Oscilator::Oscilator(float frequency, float sampleRate, Waveform waveform) : frequency(frequency), sampleRate(sampleRate), phase(0.0), waveform(waveform) {}

float	Oscilator::nextSample()
{
	float	tableIndex = (phase / (2.0 * M_PI)) * waveTables[waveform].size();
	int		index0 = static_cast<int>(tableIndex) % waveTables[waveform].size();
	int		index1 = (index0 + 1) % waveTables[waveform].size();
	float	frac = tableIndex - std::floor(tableIndex);

	float	sample = (1.0f - frac) * waveTables[waveform][index0] + frac * waveTables[waveform][index1];

	phase += (2.0 * M_PI * frequency) / sampleRate;
	if (phase >= (2.0 * M_PI))
		phase -= (2.0 * M_PI);
	return sample;
}

float	Oscilator::getFrequency() const { return frequency; }

void	Oscilator::initWaveTables(int tableSize)
{
	waveTables[Waveform::SINE].resize(tableSize);
	waveTables[Waveform::SQUARE].resize(tableSize);
	waveTables[Waveform::SAWTOOTH].resize(tableSize);
	waveTables[Waveform::TRIANGLE].resize(tableSize);

	for (int i = 0; i < tableSize; ++i)
	{
		float	phase = (static_cast<float>(i) / tableSize) * 2.0f * M_PI;
		waveTables[Waveform::SINE][i] = sin(phase);
		waveTables[Waveform::SQUARE][i] = (phase < M_PI) ? 1.0f : -1.0f;
		float	t = static_cast<float>(i) / tableSize;
		waveTables[Waveform::SAWTOOTH][i] = (2.0f * t) - 1.0f;
		waveTables[Waveform::TRIANGLE][i] = (phase < M_PI) ? (4.0f * t) - 1.0f : 3.0f - (4.0f * t);
	}
}
