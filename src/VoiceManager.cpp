#include "VoiceManager.hpp"

Envelope::Envelope() : state(ADSRState::OFF), attackTime(0.1f), decayTime(0.1f), sustainLevel(0.7f), releaseTime(0.2f), currentAmplitude(0.0f), currentTime(0.0f), lastAmplitude(0.0f) {}
Envelope::Envelope(float attack, float decay, float sustain, float release) : state(ADSRState::OFF), attackTime(attack), decayTime(decay), sustainLevel(sustain), releaseTime(release), currentAmplitude(0.0f), currentTime(0.0f), lastAmplitude(0.0f) {}

float	Envelope::nextAmplitude(float deltaTime)
{
	switch (state)
	{
		case ADSRState::ATTACK:
			currentAmplitude += deltaTime / attackTime;
			currentTime += deltaTime;
			if (currentTime >= attackTime)
			{
				currentAmplitude = 1.0f;
				state = ADSRState::DECAY;
				currentTime = 0.0f;
			}
			break;
		case ADSRState::DECAY:
			currentAmplitude -= deltaTime * (1.0f - sustainLevel) / decayTime;
			currentTime += deltaTime;
			if (currentTime >= decayTime)
			{
				currentAmplitude = sustainLevel;
				state = ADSRState::SUSTAIN;
				currentTime = 0.0f;
			}
			break;
		case ADSRState::RELEASE:
			currentAmplitude -= deltaTime * lastAmplitude / releaseTime;
			currentTime += deltaTime;
			if (currentTime >= releaseTime)
			{
				currentAmplitude = 0.0f;
				lastAmplitude = currentAmplitude;
				state = ADSRState::OFF;
				currentTime = 0.0f;
			}
			break;
		default:
			break;
	}
	return currentAmplitude;
}

void	Envelope::start()
{
	state = ADSRState::ATTACK;
	currentTime = 0.0f;
	currentAmplitude = 0.0f;
	lastAmplitude = 0.0f;
}

float	SynthVoice::nextSample() { return carrier.nextSample(modulator.nextSample() * modulationIndex) * env.nextAmplitude(1.0f / 44100.0f) * velocity; }

void	SynthVoice::noteOn(int midiNote, float modulationIndex, float modulationRatio, float frequency, float velocity, float sampleRate, Waveform waveform)
{
	this->midiNote = midiNote;
	this->velocity = velocity;
	carrier = Oscilator(frequency, sampleRate, waveform);
	modulator = Oscilator(frequency * modulationRatio, sampleRate, Waveform::SINE);
	this->modulationIndex = modulationIndex;
	env.start();
}

void	SynthVoice::noteOff() { env.state = ADSRState::RELEASE; env.currentTime = 0.0f; env.lastAmplitude = env.currentAmplitude; }

VoiceManager::VoiceManager() : maxVoices(8), sampleRate(44100.0f) { voices.resize(maxVoices); }

VoiceManager::VoiceManager(int maxVoices, float sampleRate) : maxVoices(maxVoices), sampleRate(sampleRate) { voices.resize(maxVoices); }

void	VoiceManager::noteOn(int midiNote, float modulationIndex, float modulationRatio, float frequency, float velocity, Waveform waveform)
{
	for (int i = 0; i < maxVoices; i++)
	{
		if (voices[i].env.state == ADSRState::OFF)
		{
			voices[i].noteOn(midiNote, modulationIndex, modulationRatio, frequency, velocity, sampleRate, waveform);
			return;
		}
	}

	int		oldestVoiceIndex = 0;
	float	maxTime = -1.0f;
	for (int i = 0; i < maxVoices; i++)
	{
		if (voices[i].env.state == ADSRState::RELEASE)
		{
			oldestVoiceIndex = i;
			break;
		}
		if (voices[i].env.currentTime > maxTime)
		{
			maxTime = voices[i].env.currentTime;
			oldestVoiceIndex = i;
		}
	}
	voices[oldestVoiceIndex].noteOn(midiNote, modulationIndex, modulationRatio, frequency, velocity, sampleRate, waveform);
}

void	VoiceManager::noteOff(int midiNote)
{
	for (int i = 0; i < maxVoices; i++)
		if (voices[i].midiNote == midiNote && (voices[i].env.state != ADSRState::OFF && voices[i].env.state != ADSRState::RELEASE))
			voices[i].noteOff();
}

float	VoiceManager::nextSample()
{
	float	sample = 0.0f;

	for (int i = 0; i < maxVoices; ++i)
		if (voices[i].env.state != ADSRState::OFF)
			sample += voices[i].nextSample();

	return sample / maxVoices;
}
