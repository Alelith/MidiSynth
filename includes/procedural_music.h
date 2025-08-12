#ifndef PROCEDURAL_MUSIC_H
# define PROCEDURAL_MUSIC_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <math.h>
# include <alsa/asoundlib.h>

/*========================Audio settings========================*/
# define SAMPLE_RATE 44100
# define CHANNELS 2
# define PI 3.14159265358979323846

/*========================Enharmonic equivalents (flats mapped to sharps)========================*/
# define Db Cs
# define Eb Ds
# define Gb Fs
# define Ab Gs
# define Bb As

/**
 * @brief A macro to generate a waveform based on either a musical note or a frequency.
 *
 * @param first_arg The first argument (note or frequency).
 * @param ... Additional arguments (octave for notes).
 *
 * @return A pointer to the generated waveform data (PCM samples).
 */
#define generate_wave(first_arg, ...) _Generic((first_arg), \
	int: generate_wave_by_note, \
	float: generate_wave_by_frequency, \
	double: generate_wave_by_frequency \
)(first_arg, __VA_ARGS__)

/*========================Waveform generation functions========================*/

/**
 * @brief Enumeration of musical notes (using sharp notation).
 */
typedef enum
{
	C = 0,
	Cs = 1,
	D = 2,
	Ds = 3,
	E = 4,
	F = 5,
	Fs = 6,
	G = 7,
	Gs = 8,
	A = 9,
	As = 10,
	B = 11
}	t_note;

/**
 * @brief Enumeration of waveform types.
 */
typedef enum
{
	SINE,
	SQUARE,
	SAWTOOTH,
	TRIANGLE
}	t_waveform;

/**
 * @brief Get the musical note enum from a string.
 *
 * @param str The input string representing a musical note.
 * 
 * @return The corresponding t_note enum value, or -1 if not found.
 */
t_note	get_note(const char *str);


/**
 * @brief Generate a waveform based on a musical note and its octave.
 *
 * @param note The musical note (t_note enum).
 * @param octave The octave number (e.g., 4 for middle C).
 * @param waveform The desired waveform type (e.g., SINE, SQUARE).
 * 
 * @return A pointer to the generated waveform data (PCM samples).
 */
short	*generate_wave_by_note(t_note note, int octave, t_waveform waveform, unsigned int seconds);

/**
 * @brief Generate a waveform based on a frequency.
 *
 * @param frequency The frequency in Hz.
 * @param waveform The desired waveform type (e.g., SINE, SQUARE).
 * 
 * @return A pointer to the generated waveform data (PCM samples).
 */
short	*generate_wave_by_frequency(float frequency, t_waveform waveform, unsigned int seconds);

/**
 * @brief Add multiple waveform samples together.
 *
 * @param waves The number of waveform samples to add.
 * @param ... The waveform samples to add (variadic arguments).
 *
 * @return A pointer to the combined waveform data (PCM samples).
 */
short	*add_waves(short waves, unsigned int seconds, ...);

/**
 * @brief Convert a mono audio buffer to stereo by duplicating each sample.
 *
 * @param mono The mono audio buffer (PCM samples).
 *
 * @return A pointer to the stereo audio buffer (PCM samples).
 */
short	*mono_to_stereo(const short *mono, unsigned int seconds);

#endif // PROCEDURAL_MUSIC_H