/**
 * @file TuningSys.hpp
 * @brief Microtonal tuning and frequency mapping system
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#ifndef TUNINGSYS_HPP
# define TUNINGSYS_HPP

# include <vector>
# include <string>

/**
 * @defgroup tuning_module Microtonal Tuning Module
 * @brief System for mapping MIDI notes to specific audio frequencies
 * 
 * @details This module allows the synthesizer to depart from standard
 * 12-tone equal temperament by reading microtonal Scala files and 
 * building a custom frequency lookup table mapped against MIDI note indices.
 * 
 * @section module_features_sec Features
 * - Default 12-Tone Equal Temperament (12-TET) generation
 * - Parsing and mapping of Scala (.scl) tuning files
 * - Fast frequency lookup for real-time synthesis
 * 
 * @section module_usage_sec Usage
 * Instantiate TuningSys with or without a Scala file path. Use the
 * getFrequency() command when processing MIDI Note-On events to
 * determine the target fundamental frequency of an oscillator.
 */

using std::vector;
using std::string;

/**
 * @class TuningSys
 * @brief Frequency lookup table generator and manager
 * 
 * Generates an internal lookup table spanning all 128 standard MIDI
 * note combinations, extrapolating pitch via either equal temperament
 * mathematical models or custom fractional ratios from Scala files.
 */
class TuningSys
{
	public:
		TuningSys();
		TuningSys(string tuningFile, float baseFreq = 432.0f, int midiNoteOffset = 69);
		~TuningSys();

		float	getFrequency(int note) const;
	private:
		vector<float>	notes;            ///< Precomputed zero-indexed frequency array
		float			baseFrequency = 432.0f; ///< Absolute A4 frequency for standard tuning
		int				midiNoteOffset = 69; ///< Static standard MIDI note number for A4
};

#endif /* TUNINGSYS_HPP */
