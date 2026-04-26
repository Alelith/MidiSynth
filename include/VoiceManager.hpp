#ifndef VOICEMANAGER_HPP
# define VOICEMANAGER_HPP

# include <vector>
# include <mutex>
# include "Oscilator.hpp"

using std::vector;
using std::mutex;
using std::lock_guard;

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

	float	nextSample();
	void	noteOn(int midiNote, float modulationIndex, float modulationRatio, float frequency, float sampleRate, Waveform waveform = Waveform::SINE);
	void	noteOff();
};

class VoiceManager
{
	public:
		VoiceManager();
		VoiceManager(int maxVoices, float sampleRate = 88200.0f);
		VoiceManager(const VoiceManager&);
		VoiceManager& operator=(const VoiceManager&);
		~VoiceManager() = default;

		void	noteOn(int midiNote, float modulationIndex, float modulationRatio, float frequency, Waveform waveform = Waveform::SINE);
		void	noteOff(int midiNote);
		float	nextSample();
	private:
		mutex				stateMutex;
		vector<SynthVoice>	voices;
		int					maxVoices;

		float	sampleRate = 88200.0f;
};

#endif /* VOICEMANAGER_HPP */
