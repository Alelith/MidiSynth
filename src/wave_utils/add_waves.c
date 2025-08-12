#include "procedural_music.h"

static short *combine_waves(short *wave1, short *wave2, unsigned int total_samples, unsigned int num_waves)
{
	if (!wave1 || !wave2)
		return NULL;

	float *temp_result = malloc(total_samples * sizeof(float));
	short *result = malloc(total_samples * sizeof(short));
	if (!result || !temp_result)
		return NULL;

	// Suma y normaliza
	for (unsigned int i = 0; i < total_samples; i++)
		temp_result[i] = ((float)wave1[i] + (float)wave2[i]) / num_waves;

	// Encuentra el pico máximo absoluto
	float max_peak = 0.0f;
	for (unsigned int i = 0; i < total_samples; i++) {
		float abs_val = fabsf(temp_result[i]);
		if (abs_val > max_peak)
			max_peak = abs_val;
	}

	// Calcula el factor de normalización
	float norm_factor = (max_peak > 0.0f) ? (32767.0f / max_peak) : 1.0f;

	// Aplica normalización y clamping
	for (unsigned int i = 0; i < total_samples; i++) {
		float val = temp_result[i] * norm_factor;
		if (val > 32767.0f) result[i] = 32767;
		else if (val < -32768.0f) result[i] = -32768;
		else result[i] = (short)val;
	}

	free(temp_result);
	return result;
}

short *add_waves(short waves, unsigned int seconds, ...)
{
	va_list args;
	short *result = NULL;
	short *temp = NULL;
	short *first_wave = NULL;
	unsigned int total_samples = SAMPLE_RATE * seconds;
	unsigned int num_waves = waves;

	if (waves < 1)
		return NULL;
	va_start(args, seconds);
	// Process the first wave
	first_wave = va_arg(args, short *);
	result = first_wave;

	// Process additional waves
	short *next_wave;
	while (--waves)
	{
		next_wave = va_arg(args, short *);
		temp = combine_waves(result, next_wave, total_samples, num_waves);
		if (result != first_wave)
			free(result);
		result = temp;
	}

	va_end(args);
	return result;
}