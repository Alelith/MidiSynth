/**
 * @file AudioEngine.cpp
 * @brief Implementation of the main audio engine
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#include "AudioEngine.hpp"
#include <iostream>
#include <cmath>

/**
 * @brief Constructs an AudioEngine with a specific ring buffer
 * 
 * @details Initializes PortAudio and automatically starts the stream
 * and MIDI processing with default internal tuning and synthesis settings.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] buffer Reference to the lock-free output ring buffer
 */
AudioEngine::AudioEngine(RingBuffer &buffer) : stream(nullptr), audioBuffer(buffer) { Pa_Initialize(); start(); }

/**
 * @brief Constructs an AudioEngine with custom synthesis and tuning parameters
 * 
 * @details Configures the engine for Frequency Modulation with a custom mapping,
 * applies a custom Scala tuning file, and then starts audio streams.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] buffer Reference to the lock-free output ring buffer
 * @param[in] modulationIndex Intensity of the FM modulation
 * @param[in] modulationRatio Ratio of carrier to modulator frequency
 * @param[in] tuningFile Path to the Scala tuning scale definition file
 * @param[in] baseFreq Base frequency mapping in Hz (A4)
 * @param[in] midiNoteOffset MIDI note number matching the base frequency
 */
AudioEngine::AudioEngine(RingBuffer &buffer, float modulationIndex, float modulationRatio, string tuningFile, float baseFreq, int midiNoteOffset) : stream(nullptr), audioBuffer(buffer), modulationIndex(modulationIndex), modulationRatio(modulationRatio), tuningSys(tuningFile, baseFreq, midiNoteOffset) { Pa_Initialize(); start(); }

/**
 * @brief Destroys the AudioEngine
 * 
 * @details Ensures streams are safely stopped and PortAudio is properly terminated.
 * 
 * @ingroup audio_engine_module
 */
AudioEngine::~AudioEngine() { stop(); Pa_Terminate(); }

/**
 * @brief Starts the synthesis and MIDI processes
 * 
 * @details Opens a virtual MIDI input port and starts the PortAudio output
 * stream, routing data through VoiceManager. Returns immediately.
 * 
 * @ingroup audio_engine_module
 * 
 * @return stream startup status
 * @retval true Success or stream was already running
 * @retval false Failed to open or start the audio stream
 */
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

/**
 * @brief Stops and releases synthesis and MIDI processes
 * 
 * @details Stops the active PortAudio stream and closes the handles.
 * Allows the AudioEngine to be gracefully shut down cleanly.
 * 
 * @ingroup audio_engine_module
 * 
 * @return Shutdown status
 * @retval true Successfully stopped and closed
 * @retval false Problem stopping or closing the audio stream
 */
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

/**
 * @brief Triggers a note to begin synthesis output
 * 
 * @details Commands the VoiceManager to handle a new voice start based
 * on frequency and velocity inputs along with current FM parameters.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] midiNote The standard MIDI note value activated
 * @param[in] frequency Target fundamental frequency in Hz (scaled via TuningSys)
 * @param[in] velocity Impact level dynamically controlling volume amplitude
 * @param[in] waveform Base waveform generated (e.g., Sine, Square, Saw)
 */
void	AudioEngine::noteOn(int midiNote, float frequency, float velocity, Waveform waveform) { voices.noteOn(midiNote, modulationIndex.load(), modulationRatio.load(), frequency, velocity, waveform); }

/**
 * @brief Finishes a currently active synthesized note
 * 
 * @details Asks the VoiceManager to enter the release phase of the ADSR
 * envelope for the corresponding allocated voice.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] midiNote Note index that has just been released
 */
void	AudioEngine::noteOff(int midiNote) { voices.noteOff(midiNote); }

/**
 * @brief Adjusts global FM modulation index parameter safely
 * 
 * @details Atomic wrapper allowing real-time FM updates across threads.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] index Intensity modifier to be applied per voice FM routing
 */
void	AudioEngine::setModulationIndex(float index) { modulationIndex.store(index); }

/**
 * @brief Adjusts global FM ratio parameter safely
 * 
 * @details Atomic wrapper allowing real-time ratio updates without locks.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] ratio Ratio of Modulator to Carrier wave frequency
 */
void	AudioEngine::setModulationRatio(float ratio) { modulationRatio.store(ratio); }

/**
 * @brief Core real-time OS-level PortAudio callback implementation
 * 
 * @details Responsible directly for filling the provided output buffer
 * with new audio samples retrieved via processAudio member format.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] inputBuffer Audio frame pointer passed by OS (unused here)
 * @param[out] outputBuffer Audio frame destination to produce audio signals
 * @param[in] framesPerBuffer Amount of calculated frames required this callback
 * @param[in] timeInfo Exact stream timing metadata struct pointer
 * @param[in] statusFlags Diagnostics from PortAudio callback handler
 * @param[in,out] userData Casted generic AudioEngine component instance
 * @return paContinue if successful callback logic processing
 * @retval paContinue Continue processing buffers indefinitely 
 * @retval paComplete Done generating audio and ready to close
 */
int	AudioEngine::paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	AudioEngine	*engine = static_cast<AudioEngine*>(userData);
	return engine->processAudio(static_cast<float*>(outputBuffer), framesPerBuffer);
}

/**
 * @brief Event callback wrapper triggered upon an incoming MIDI message
 * 
 * @details Retrieves note and velocity parameters from MIDI format then
 * maps standard notes into frequency using TuningSys before pushing
 * into MIDI Queue for synchronous execution later.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[in] timeStamp Exact hardware system time of event receipt 
 * @param[in] message Raw byte vector of latest RtMidi packet
 * @param[in,out] userData The owning AudioEngine context
 */
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

/**
 * @brief Processes buffered MIDI and processes synthesized PCM signals
 * 
 * @details Drains internal event queues synchronised with the Audio thread
 * and evaluates next state from VoiceManager, saving frames.
 * 
 * @ingroup audio_engine_module
 * 
 * @param[out] outputBuffer Destination layout pointer populated with floats
 * @param[in] framesPerBuffer How many samples to generate
 * @return paContinue OS signal instruction success 
 * @retval paContinue Portaudio continue operation code
 */
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
