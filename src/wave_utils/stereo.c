#include "procedural_music.h"

short *mono_to_stereo(const short *mono, unsigned int seconds)
{
    if (!mono)
        return NULL;

    unsigned int total_samples = SAMPLE_RATE * seconds;
    short *stereo = malloc(total_samples * 2 * sizeof(short));
    if (!stereo)
        return NULL;

    for (unsigned int i = 0; i < total_samples; i++) {
        stereo[2*i] = mono[i];     // Left channel
        stereo[2*i+1] = mono[i];   // Right channel
    }
    return stereo;
}
