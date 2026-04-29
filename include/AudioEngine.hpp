/**
 * @mainpage MidiSynth Documentation
 * 
 * @section intro_sec Introduction
 * A robust and flexible polyphonic synthesizer engine with MIDI capabilities.
 * 
 * @section features_sec Features
 * - Real-time Audio generation via PortAudio
 * - MIDI input processing via RtMidi
 * - Custom Polyphony and Voice Management
 * - Microtonal tuning support via Scala files
 * 
 * @section architecture_sec Architecture
 * The system is built around a central AudioEngine that coordinates
 * MIDI input processing, Voice management for polyphony, and an
 * Oscillator system for waveform generation and modulation.
 * 
 * @ref audio_engine_module Module Overview
 * 
 * @section implementation_sec Implementation
 * The engine uses a lock-free RingBuffer for concurrent audio processing
 * and a lock-free MidiQueue to pass MIDI events safely from the callback
 * thread to the processing thread.
 * 
 * @section dependencies_sec Dependencies
 * - PortAudio 
 * - RtMidi
 * 
 * @section install_sec Installation
 * Compile the project using CMake. Make sure you have the required
 * dependencies installed on your system.
 * 
 * @section usage_sec Usage
 * Create an instance of the AudioEngine passing a RingBuffer, and call start().
 * 
 * @section contrib_sec Contributing
 * Feature requests and bug reports are welcome via issue tracker.
 * 
 * @section license_sec License
 * MIT License
 */

/**
 * @file AudioEngine.hpp
 * @brief Core synthesizer engine and thread management
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#ifndef AUDIOENGINE_HPP
# define AUDIOENGINE_HPP

# include <atomic>
# include <portaudio.h>
# include <rtmidi/RtMidi.h>
# include "VoiceManager.hpp"
# include "TuningSys.hpp"
# include "RingBuffer.hpp"
# include "MidiQueue.hpp"

/**
 * @defgroup audio_engine_module Core Audio Engine
 * @brief Central module for audio processing and MIDI routing
 * 
 * @details This module manages the lifecycle of the audio stream,
 * handles MIDI callbacks, and synthesizes audio frames.
 * 
 * @section module_features_sec Features
 * - Stream lifecycle management (start/stop)
 * - Thread-safe modulation parameter updates
 * - Built-in MIDI routing to voice manager
 * 
 * @section module_usage_sec Usage
 * Use this module to setup your main audio loop and map MIDI input 
 * devices for real-time synthesis.
 */

using std::atomic;

/**
 * @class AudioEngine
 * @brief Main synthesizer and audio coordination class
 * 
 * The AudioEngine class bridges external inputs (MIDI) with internal
 * sound synthesis tools (VoiceManager, Oscillators) and pipes the
 * generated sound into a real-time PortAudio stream.
 */
class AudioEngine
{
	public:
		AudioEngine(RingBuffer &buffer);
		AudioEngine(RingBuffer &buffer, float modulationIndex, float modulationRatio, string tuningFile, float baseFreq = 432.0f, int midiNoteOffset = 69);
		~AudioEngine();

		bool	start();
		bool	stop();

		void	noteOn(int midiNote, float frequency, float velocity, Waveform waveform = Waveform::SINE);
		void	noteOff(int midiNote);

		void	setModulationIndex(float index);
		void	setModulationRatio(float ratio);
	private:
		PaStream	*stream;      ///< PortAudio stream pointer
		float		sampleRate = 44100.0f; ///< Operating sample rate
		TuningSys	tuningSys;    ///< Microtonal tuning system manager

		RtMidiIn	midiIn;       ///< RtMidi input handler

		VoiceManager	voices;       ///< Polyphonic voice coordinator
		RingBuffer		&audioBuffer; ///< Shared buffer for outputs
		MidiQueue		midiQueue;    ///< Lock-free queue for MIDI events

		atomic<float>	modulationIndex; ///< Thread-safe FM index parameter
		atomic<float>	modulationRatio; ///< Thread-safe FM ratio parameter

		static int	paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
		static void	midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
		int			processAudio(float *outputBuffer, unsigned long framesPerBuffer);
};

#endif /* AUDIOENGINE_HPP */
