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
	Oscilator	osc;
	Envelope	env;

	void	noteOn(float frequency, float sampleRate, Waveform waveform = Waveform::SINE);
	void	noteOff();
};

class VoiceManager
{
	public:
		VoiceManager();
		VoiceManager(int maxVoices, float sampleRate = 88200.0f);
		~VoiceManager() = default;

		void	noteOn(float frequency, Waveform waveform = Waveform::SINE);
		void	noteOff(float frequency);
		float	nextSample();
	private:
		vector<SynthVoice>	voices;
		int					maxVoices;

		float	sampleRate = 88200.0f;
};

#endif /* VOICEMANAGER_HPP */
