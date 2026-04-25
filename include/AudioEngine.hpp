#ifndef AUDIOENGINE_HPP
# define AUDIOENGINE_HPP

# include <atomic>
# include <portaudio.h>
# include <rtmidi/RtMidi.h>
# include "VoiceManager.hpp"
# include "TuningSys.hpp"

using std::atomic;

class AudioEngine
{
	public:


		AudioEngine();
		AudioEngine(float modulationIndex, float modulationRatio, string tuningFile, float baseFreq = 432.0f, int midiNoteOffset = 69);
		~AudioEngine();

		bool	start();
		bool	stop();

		void	noteOn(float frequency, Waveform waveform = Waveform::SINE);
		void	noteOff(float frequency);

		void	setModulationIndex(float index);
		void	setModulationRatio(float ratio);
	private:
		PaStream	*stream;
		float		sampleRate = 88200.0f;
		TuningSys	tuningSys;

		RtMidiIn	midiIn;

		VoiceManager	voices;

		atomic<float>	modulationIndex;
		atomic<float>	modulationRatio;

		static int	paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
		static void	midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
		int			processAudio(float *outputBuffer, unsigned long framesPerBuffer);
};

#endif /* AUDIOENGINE_HPP */
