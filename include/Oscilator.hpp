/**
 * @file Oscilator.hpp
 * @brief Waveform generator and oscillator definitions
 * 
 * @author Lilith Estévez Boeta 
 * @date 2026-04-28
 */

#ifndef OSCILATOR_HPP
# define OSCILATOR_HPP

# include <map>
# include <vector>

/**
 * @defgroup oscilator_module Oscillator Synthesis Module
 * @brief Core waveform generation and modulation mechanisms
 * 
 * @details Provides the fundamental tools for generating sound waves using
 * pre-calculated lookup tables (WaveTables) to optimize performance.
 * 
 * @section module_features_sec Features
 * - Support for Sine, Square, Sawtooth, and Triangle waves
 * - Efficient interpolation via pre-computed WaveTables
 * - Frequency modulation support built into the sampling logic
 * 
 * @section module_usage_sec Usage
 * First call Oscilator::initWaveTables() globally to generate lookup tables.
 * Instantiate the class and continuously call nextSample() per audio frame.
 */

using std::map;
using std::vector;

/**
 * @enum Waveform
 * @brief Synthesizer primitive wave shapes
 * 
 * Enumerates the standard analog modeling waveforms supported
 * by the oscillator generation algorithm.
 */
enum class Waveform
{
	SINE,      ///< Pure sinusoidal wave
	SQUARE,    ///< Hard-edged bipolar binary wave
	SAWTOOTH,  ///< Linear ramp decaying wave
	TRIANGLE   ///< Linear bipolar triangular wave
};

/**
 * @class Oscilator
 * @brief Generator for primitive periodic audio waveforms
 * 
 * This class calculates and tracks the phase of an acoustic wave
 * outputting high-efficiency interpolated frames from a static WaveTable.
 */
class Oscilator
{
	public:
		Oscilator() = default;
		Oscilator(float frequency, float sampleRate, Waveform waveform = Waveform::SINE);

		float	nextSample(float modulation = 0.0f);
		float	getFrequency() const;

		static void	initWaveTables(int tableSize = 2048);
	private:
		float		frequency;    ///< Target oscillation frequency in Hz
		float		sampleRate;   ///< Master system playback sample rate
		double		phase;        ///< Current cyclic location in radians
		Waveform	waveform;     ///< Selected shape to synthesize

		static vector<float> waveTables[4]; ///< Precomputed wave data arrays
};

#endif /* OSCILATOR_HPP */
