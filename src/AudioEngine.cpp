#include "AudioEngine.hpp"
#include <iostream>
#include <cmath>

AudioEngine::AudioEngine(RingBuffer &buffer) : stream(nullptr), audioBuffer(buffer) { Pa_Initialize(); start(); }

AudioEngine::AudioEngine(RingBuffer &buffer, float modulationIndex, float modulationRatio, string tuningFile, float baseFreq, int midiNoteOffset) : stream(nullptr), audioBuffer(buffer), modulationIndex(modulationIndex), modulationRatio(modulationRatio), tuningSys(tuningFile, baseFreq, midiNoteOffset) { Pa_Initialize(); start(); }

AudioEngine::~AudioEngine() { stop(); Pa_Terminate(); }

bool	AudioEngine::start()
{
	if (stream)
		return true;
	
	Oscilator::initWaveTables();

	voices = VoiceManager(64, sampleRate);

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

void	AudioEngine::noteOn(int midiNote, float frequency, float velocity, Waveform waveform) { voices.noteOn(midiNote, modulationIndex.load(), modulationRatio.load(), frequency, velocity, waveform); }

void	AudioEngine::noteOff(int midiNote) { voices.noteOff(midiNote); }

void	AudioEngine::setModulationIndex(float index) { modulationIndex.store(index); }

void	AudioEngine::setModulationRatio(float ratio) { modulationRatio.store(ratio); }

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
	unsigned char	statusByte = message->at(0);
	unsigned char	status = statusByte & 0xF0;
	unsigned char	channel = statusByte & 0x0F;

	if (channel == 9) // Ignore percussion channel
		return;

	unsigned char	note = message->at(1);
	unsigned char	velocity = message->at(2);

	MidiEvent	ev;
	ev.note = note;

	if (status == 0x90 && velocity > 0) // Note On
	{
		ev.isOn = true;
		ev.frequency = engine->tuningSys.getFrequency(note);

		float velocityNorm = velocity / 127.0f;
		ev.velocity = velocityNorm * velocityNorm; // Apply a simple curve for better expressiveness

		engine->midiQueue.push(ev);
	}
	else if ((status == 0x80) || (status == 0x90 && velocity == 0)) // Note Off
	{
		ev.isOn = false;
		engine->midiQueue.push(ev);
	}
}

int	AudioEngine::processAudio(float *outputBuffer, unsigned long framesPerBuffer)
{
	MidiEvent	ev;
	while (midiQueue.pop(ev))
	{
		if (ev.isOn)
			noteOn(ev.note, ev.frequency, ev.velocity, Waveform::SINE);
		else
			noteOff(ev.note);
	}

	for (unsigned long i = 0; i < framesPerBuffer; ++i)
	{
		float	sample = voices.nextSample();
		sample *= 7.0f;
		sample = std::tanh(sample);
		audioBuffer.write(&sample, 1);
		outputBuffer[i * 2] = sample;
		outputBuffer[i * 2 + 1] = sample;
	}
	return paContinue;
}
