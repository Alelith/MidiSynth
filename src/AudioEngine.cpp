#include "AudioEngine.hpp"
#include <iostream>
#include <cmath>

AudioEngine::AudioEngine() : stream(nullptr) { Pa_Initialize(); start(); }

AudioEngine::AudioEngine(string tuningFile, float baseFreq, int midiNoteOffset) : stream(nullptr), tuningSys(tuningFile, baseFreq, midiNoteOffset) { Pa_Initialize(); start(); }

AudioEngine::~AudioEngine() { stop(); Pa_Terminate(); }

bool	AudioEngine::start()
{
	if (stream)
		return true;
	
	Oscilator::initWaveTables();

	voices = VoiceManager(8);

	midiIn.openVirtualPort("My Synth MIDI Input");
	midiIn.setCallback(midiCallback, this);

	PaStreamParameters	outputParameters;
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = nullptr;

	PaError	err = Pa_OpenStream(&stream, nullptr, &outputParameters, sampleRate, 256, paClipOff, paCallback, this);
	if (err != paNoError)
	{
		std::cerr << "Error opening stream: " << Pa_GetErrorText(err) << std::endl;
		return false;
	}

	err = Pa_StartStream(stream);
	if (err != paNoError)
	{
		std::cerr << "Error starting stream: " << Pa_GetErrorText(err) << std::endl;
		return false;
	}

	return true;
}

bool	AudioEngine::stop()
{
	if (!stream)
		return true;

	PaError	err = Pa_StopStream(stream);
	if (err != paNoError)
	{
		std::cerr << "Error stopping stream: " << Pa_GetErrorText(err) << std::endl;
		return false;
	}

	err = Pa_CloseStream(stream);
	stream = nullptr;
	if (err != paNoError)
	{
		std::cerr << "Error closing stream: " << Pa_GetErrorText(err) << std::endl;
		return false;
	}
	return true;
}

void	AudioEngine::noteOn(float frequency, Waveform waveform) { voices.noteOn(frequency, waveform); }

void	AudioEngine::noteOff(float frequency) { voices.noteOff(frequency); }

int	AudioEngine::paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	AudioEngine	*engine = static_cast<AudioEngine*>(userData);
	return engine->processAudio(static_cast<float*>(outputBuffer), framesPerBuffer);
}

void	AudioEngine::midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData)
{
	if (message->size() < 3)
		return;

	AudioEngine	*engine = static_cast<AudioEngine*>(userData);
	unsigned char	status = message->at(0) & 0xF0;
	unsigned char	note = message->at(1);
	unsigned char	velocity = message->at(2);

	if (status == 0x90 && velocity > 0) // Note On
	{
		float	frequency = engine->tuningSys.getFrequency(note);
		std::cout << "Note On: " << (int)note << " Velocity: " << (int)velocity << " Frequency: " << frequency << " Hz" << std::endl;
		engine->noteOn(frequency, Waveform::SINE);
	}
	else if ((status == 0x80) || (status == 0x90 && velocity == 0)) // Note Off
	{
		float	frequency = engine->tuningSys.getFrequency(note);
		std::cout << "Note Off: " << (int)note << " Velocity: " << (int)velocity << " Frequency: " << frequency << " Hz" << std::endl;
		engine->noteOff(frequency);
	}
}

int	AudioEngine::processAudio(float *outputBuffer, unsigned long framesPerBuffer)
{
	for (unsigned long i = 0; i < framesPerBuffer; ++i)
	{
		float	sample = voices.nextSample();
		outputBuffer[i * 2] = sample;
		outputBuffer[i * 2 + 1] = sample;
	}
	return paContinue;
}
