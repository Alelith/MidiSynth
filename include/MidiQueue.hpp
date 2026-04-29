/**
 * @file MidiQueue.hpp
 * @brief Lock-free MIDI event queue management
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#ifndef MIDIQUEUE_HPP
# define MIDIQUEUE_HPP

# include <atomic>

/**
 * @defgroup midi_queue_module Lock-Free MIDI Queue
 * @brief Thread-safe passing of MIDI events to the audio thread
 * 
 * @details This module provides a circular, lock-free queue tailored for
 * reliable real-time transmission of MIDI events between the asynchronous
 * MIDI callback and the strict time-constrained audio rendering routine.
 * 
 * @section module_features_sec Features
 * - Fixed 1024-element underlying array
 * - Wait-free push and pop semantics
 * - Atomic memory ordering guarantees
 * 
 * @section module_usage_sec Usage
 * The MIDI thread pushes new MidiEvent structures while the audio thread
 * continually polls pop() prior to rendering the next sample batch.
 */

using std::atomic;

/**
 * @struct MidiEvent
 * @brief Standardized synthesizer MIDI command package
 * 
 * Encapsulates all relevant data for triggering or releasing a voice
 * in the synthesis engine.
 */
struct MidiEvent
{
	int		note;       ///< Extracted standard MIDI note number
	float	frequency;  ///< Computed target ideal hz frequency
	bool	isOn;       ///< Flag evaluating ON (true) or OFF (false) state
	float	velocity;   ///< Normalized strike force affecting amplitude
};

/**
 * @class MidiQueue
 * @brief Concurrent ring buffer for MIDI event dispatch
 * 
 * Establishes a synchronized one-directional pipeline enabling safe 
 * passing of MidiEvents into the processing context without locks.
 */
class MidiQueue
{
	public:
		MidiQueue();

		bool	push(const MidiEvent& event);
		bool	pop(MidiEvent& event);
	private:
		MidiEvent	events[1024];  ///< Internal discrete fixed array cache
		atomic<int>	writeIndex;    ///< Lock-free current insertion index
		atomic<int>	readIndex;     ///< Lock-free current extraction index
};

#endif /* MIDIQUEUE_HPP */