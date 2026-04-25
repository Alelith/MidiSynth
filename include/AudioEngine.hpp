#ifndef AUDIOENGINE_HPP
# define AUDIOENGINE_HPP

# include <portaudio.h>
# include <rtmidi/RtMidi.h>
# include "VoiceManager.hpp"

class AudioEngine
{
	public:
		AudioEngine();
		~AudioEngine();

		bool	start();
		bool	stop();

		void	noteOn(float frequency, Waveform waveform = Waveform::SINE);
		void	noteOff(float frequency);
	private:
		PaStream	*stream;
		float		sampleRate = 88200.0f;
		float		frequency = 432.0f; // A4 note

		RtMidiIn	midiIn;

		VoiceManager	voices;

		static int	paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
		static void	midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
		int			processAudio(float *outputBuffer, unsigned long framesPerBuffer);
};

#endif /* AUDIOENGINE_HPP */
