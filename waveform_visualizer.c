#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define WAVE_SAMPLES 256
#define GRAPH_WIDTH 60

// Definiciones de enum
typedef enum {
    NOTE_C,
    NOTE_Cs,
    NOTE_D,
    NOTE_Ds,
    NOTE_E,
    NOTE_F,
    NOTE_Fs,
    NOTE_G,
    NOTE_Gs,
    NOTE_A,
    NOTE_As,
    NOTE_B
} t_note;

typedef enum {
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAW
} t_wave;

// Función auxiliar para frecuencia
float get_frequency(t_note note, int octave) {
    int semitone_offset = note - NOTE_A + (octave - 4) * 12;
    return 440.0f * powf(2.0f, semitone_offset / 12.0f);
}

// Generar la onda
void get_wave(t_note note, t_wave wave, int octave, float volume, int wave_out[WAVE_SAMPLES]) {
    for (int i = 0; i < WAVE_SAMPLES; i++) {
        float position = (float)i / WAVE_SAMPLES;
        float sample_value;

        switch (wave) {
            case WAVE_SQUARE:
                sample_value = (position < 0.5f) ? 1.0f : -1.0f;
                break;
            case WAVE_TRIANGLE:
                sample_value = 4.0f * fabsf(position - 0.5f) - 1.0f;
                break;
            case WAVE_SAW:
                sample_value = 2.0f * position - 1.0f;
                break;
            default:
                sample_value = 0.0f;
                break;
        }

        wave_out[i] = (int)((sample_value * volume * 127.0f) + 128.0f);
    }
}

// Dibuja la gráfica de la onda
void draw_wave(int wave[WAVE_SAMPLES]) {
    for (int i = 0; i < WAVE_SAMPLES; i += (WAVE_SAMPLES / GRAPH_WIDTH)) {
        int value = wave[i];
        int position = (value * GRAPH_WIDTH) / 255;

        printf("|");
        for (int j = 0; j < GRAPH_WIDTH; j++) {
            if (j == position)
                printf("*");
            else
                printf(" ");
        }
        printf("|\n");
    }
}

// Programa principal
int main() {
    int octave = 4;
    float volume = 1.0f;
    int waveform[WAVE_SAMPLES];

    get_wave(NOTE_A, WAVE_SQUARE, octave, volume, waveform);

    printf("Visualización de la onda cuadrada (A4):\n");
    draw_wave(waveform);

    return 0;
}
