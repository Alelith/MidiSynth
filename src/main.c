#include "procedural_music.h"

void play_sound(short *samples, unsigned int seconds)
{
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = SAMPLE_RATE;
    int channels = CHANNELS;
    int frames = 1024;
    int rc;

    rc = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "No se pudo abrir el dispositivo PCM: %s\n", snd_strerror(rc));
        return;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, 0);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, (snd_pcm_uframes_t *)&frames, 0);

    rc = snd_pcm_hw_params(pcm_handle, params);
    if (rc < 0) {
        fprintf(stderr, "No se pudo configurar los parámetros HW: %s\n", snd_strerror(rc));
        snd_pcm_close(pcm_handle);
        return;
    }

    unsigned int total_frames = sample_rate * seconds;
    unsigned int frames_written = 0;
    while (frames_written < total_frames) {
        unsigned int frames_to_write = frames;
        if (frames_written + frames > total_frames)
            frames_to_write = total_frames - frames_written;
        rc = snd_pcm_writei(pcm_handle, samples + frames_written * channels, frames_to_write);
        if (rc == -EPIPE) {
            snd_pcm_prepare(pcm_handle);
            continue;
        } else if (rc < 0) {
            fprintf(stderr, "Error al escribir en PCM: %s\n", snd_strerror(rc));
            break;
        }
        frames_written += frames_to_write;
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
}

int main(void)
{
    short *samples;
	short *stereo;

    return 0;
}