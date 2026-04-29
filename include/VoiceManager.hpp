/**
 * @file VoiceManager.hpp
 * @brief Polyphony and voice allocation management
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#ifndef VOICEMANAGER_HPP
# define VOICEMANAGER_HPP

# include <vector>
# include "Oscilator.hpp"

/**
 * @defgroup voice_module Voice Management Module
 * @brief Polyphonic voice allocation and ADSR handling
 * 
 * @details This module contains components related to individual synthesis voices,
 * including their envelopes (ADSR) and the manager that implements polyphonic
 * stealing and allocation logic.
 * 
 * @section module_features_sec Features
 * - ADSR envelope generation
 * - FM synthesis per-voice
 * - Polyphonic voice stealing algorithm
 * 
 * @section module_usage_sec Usage
 * The VoiceManager must be instantiated with a pre-allocated number of maximum
 * voices. Then call noteOn/noteOff dynamically and retrieve audio via nextSample().
 */

using std::vector;

/**
 * @enum ADSRState
 * @brief States of the ADSR envelope
 * 
 * This enumeration defines the lifecycle phases of a note's envelope.
 */
enum class ADSRState
{
	ATTACK,   ///< Raising amplitude phase
	DECAY,    ///< Falling amplitude early phase
	SUSTAIN,  ///< Holding amplitude phase 
	RELEASE,  ///< Fading amplitude after note off
	OFF       ///< Idle inactive state
};

/**
 * @struct Envelope
 * @brief Amplitude envelope generator (ADSR)
 * 
 * Generates an amplitude multiplier varying over time according to Attack,
 * Decay, Sustain, and Release parameters.
 */
struct Envelope
{
	ADSRState	state;          ///< Current phase of the envelope
	float		attackTime;     ///< Time required for attack phase (seconds)
	float		decayTime;      ///< Time required for decay phase (seconds)
	float		sustainLevel;   ///< Held multiplier during sustain (0.0 - 1.0)
	float		releaseTime;    ///< Time required for release phase (seconds)

	float		currentTime;      ///< Local timer for the current phase
	float		currentAmplitude; ///< Amplitude output of the generator

	float		lastAmplitude;    ///< Memory for smooth phase transitions

	Envelope();
	Envelope(float attack, float decay, float sustain, float release);

	float	nextAmplitude(float deltaTime);
	void	start();
};

/**
 * @struct SynthVoice
 * @brief Single polyphonic synthesis voice
 * 
 * Groups carrier and modulator oscillators with an amplitude envelope to create
 * one independent synthesis stream capable of performing frequency modulation.
 */
struct SynthVoice
{
	Oscilator	carrier;          ///< Primary waveform generator
	Oscilator	modulator;        ///< Frequency-modulating waveform generator
	float		modulationIndex;  ///< Intensity of modulation
	Envelope	env;              ///< ADSR amplitude generator

	int		midiNote;             ///< Assigned standard MIDI note 
	float	velocity;             ///< Output volume scalar

	float	nextSample();
	void	noteOn(int midiNote, float modulationIndex, float modulationRatio, float frequency, float velocity, float sampleRate, Waveform waveform = Waveform::SINE);
	void	noteOff();
};

/**
 * @class VoiceManager
 * @brief Coordinator for polyphonic synthesis
 * 
 * Manages an array of SynthVoice instances, handling multi-note capabilities,
 * routing incoming notes to idle voices, and gracefully stealing the oldest
 * voices if polyphony limits are reached.
 */
class VoiceManager
{
	public:
		VoiceManager();
		VoiceManager(int maxVoices, float sampleRate = 44100.0f);
		~VoiceManager() = default;

		void	noteOn(int midiNote, float modulationIndex, float modulationRatio, float frequency, float velocity, Waveform waveform = Waveform::SINE);
		void	noteOff(int midiNote);
		float	nextSample();
	private:
		vector<SynthVoice>	voices;     ///< Internal pool of allocated voices
		int					maxVoices;  ///< Hard limit of active simultaneous voices

		float	sampleRate = 44100.0f;  ///< Global system sample rate
};

#endif /* VOICEMANAGER_HPP */
