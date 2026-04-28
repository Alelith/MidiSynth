#ifndef VOICEMANAGER_HPP
# define VOICEMANAGER_HPP

# include <vector>
# include "Oscilator.hpp"

using std::vector;

enum class ADSRState
{
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE,
	OFF
};

struct Envelope
{
	ADSRState	state;
	float		attackTime;
	float		decayTime;
	float		sustainLevel;
	float		releaseTime;

	float		currentTime;
	float		currentAmplitude;

	float		lastAmplitude;

	Envelope();
	Envelope(float attack, float decay, float sustain, float release);

	float	nextAmplitude(float deltaTime);
	void	start();
};

struct SynthVoice
{
	Oscilator	carrier;
	Oscilator	modulator;
	float		modulationIndex;
	Envelope	env;

	int		midiNote;
	float	velocity;

	float	nextSample();
	void	noteOn(int midiNote, float modulationIndex, float modulationRatio, float frequency, float velocity, float sampleRate, Waveform waveform = Waveform::SINE);
	void	noteOff();
};

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
		vector<SynthVoice>	voices;
		int					maxVoices;

		float	sampleRate = 44100.0f;
};

#endif /* VOICEMANAGER_HPP */
