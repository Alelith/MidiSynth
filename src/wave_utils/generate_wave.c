#include "procedural_music.h"

/**
* @brief Convert a musical note and octave to its corresponding frequency.
*
* @param note The musical note (t_note enum).
* @param octave The octave number (e.g., 4 for middle C).
* @return The frequency in Hz.
*/
static float note_to_frequency(t_note note, int octave)
{
	// Base frequencies for octave 4 (A4 = 440 Hz is the standard)
	static const float base_frequencies[] = {
		261.63f,  // C4
		277.18f,  // C#4/Db4
		293.66f,  // D4
		311.13f,  // D#4/Eb4
		329.63f,  // E4
		349.23f,  // F4
		369.99f,  // F#4/Gb4
		392.00f,  // G4
		415.30f,  // G#4/Ab4
		440.00f,  // A4
		466.16f,  // A#4/Bb4
		493.88f   // B4
	};

	// Calculate the frequency for the desired octave
	// Each octave doubles the frequency, so we use powers of 2
	float frequency = base_frequencies[note] * powf(2.0f, octave - 4);
	
	return frequency;
}

short *generate_wave_by_note(t_note note, int octave, t_waveform waveform, unsigned int seconds)
{
	float frequency = note_to_frequency(note, octave);
	return generate_wave_by_frequency(frequency, waveform, seconds);
}

short *generate_wave_by_frequency(float frequency, t_waveform waveform, unsigned int seconds)
{
	unsigned int total_samples = SAMPLE_RATE * seconds;
	short *samples = malloc(total_samples * sizeof(short));
	if (!samples)
		return NULL;

	for (unsigned int i = 0; i < total_samples; i++)
	{
		float t = (float)i / SAMPLE_RATE;
		switch (waveform)
		{
		case SINE:
			samples[i] = (short)(32767 * sinf(2.0f * PI * frequency * t));
			break;
		case SQUARE:
			samples[i] = (short)(32767 * (sinf(2.0f * PI * frequency * t) >= 0 ? 1 : -1));
			break;
		case SAWTOOTH:
			samples[i] = (short)(32767 * (2.0f * (t * frequency - floorf(t * frequency + 0.5f))));
			break;
		case TRIANGLE:
			samples[i] = (short)(32767 * (1.0f - fabsf(2.0f * (t * frequency - floorf(t * frequency + 0.5f)))));
			break;
		}
	}

	return samples;
}
