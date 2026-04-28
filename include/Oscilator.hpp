#ifndef OSCILATOR_HPP
# define OSCILATOR_HPP

# include <map>
# include <vector>

using std::map;
using std::vector;

enum class Waveform
{
	SINE,
	SQUARE,
	SAWTOOTH,
	TRIANGLE
};

class Oscilator
{
	public:
		Oscilator() = default;
		Oscilator(float frequency, float sampleRate, Waveform waveform = Waveform::SINE);

		float	nextSample(float modulation = 0.0f);
		float	getFrequency() const;

		static void	initWaveTables(int tableSize = 2048);
	private:
		float		frequency;
		float		sampleRate;
		double		phase;
		Waveform	waveform;

		static vector<float> waveTables[4];
};

#endif /* OSCILATOR_HPP */
