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

void	SynthVoice::noteOn(float frequency, float sampleRate, Waveform waveform)
{
	osc = Oscilator(frequency, sampleRate, waveform);
	env.start();
}

void	SynthVoice::noteOff() { env.state = ADSRState::RELEASE; env.currentTime = 0.0f; env.lastAmplitude = env.currentAmplitude; }

VoiceManager::VoiceManager() : maxVoices(8), sampleRate(88200.0f) { voices.resize(maxVoices); }

VoiceManager::VoiceManager(int maxVoices, float sampleRate) : maxVoices(maxVoices), sampleRate(sampleRate) { voices.resize(maxVoices); }

void	VoiceManager::noteOn(float frequency, Waveform waveform)
{
	for (int i = 0; i < maxVoices; i++)
	{
		if (voices[i].env.state == ADSRState::OFF)
		{
			voices[i].noteOn(frequency, sampleRate, waveform);
			break;
		}
	}
}

void	VoiceManager::noteOff(float frequency)
{
	for (int i = 0; i < maxVoices; i++)
	{
		if (voices[i].osc.getFrequency() == frequency && voices[i].env.state != ADSRState::OFF)
		{
			voices[i].noteOff();
			break;
		}
	}
}

float	VoiceManager::nextSample()
{
	float	sample = 0.0f;

	for (int i = 0; i < maxVoices; ++i)
		if (voices[i].env.state != ADSRState::OFF)
			sample += voices[i].osc.nextSample() * voices[i].env.nextAmplitude(1.0f / sampleRate);

	return sample / maxVoices;
}
