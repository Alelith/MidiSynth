# Procedural Music Generator

This project provides a simple C library and demo for generating, combining, and playing procedural audio waveforms using ALSA on Linux.

## Features
- **Waveform Generation**: Create PCM samples for musical notes or arbitrary frequencies using sine, square, sawtooth, or triangle waveforms.
- **Wave Addition**: Combine multiple waveforms to create chords or complex sounds, with automatic normalization to avoid clipping.
- **Mono to Stereo Conversion**: Convert mono PCM buffers to stereo by duplicating each sample to both channels.
- **Audio Playback**: Play generated waveforms through ALSA, supporting stereo output and configurable duration.

## Usage
- Generate a waveform for a note or frequency.
- Combine several waveforms to create chords.
- Convert mono waveforms to stereo.
- Play the resulting sound using ALSA.

## Requirements
- Linux
- ALSA development libraries (`libasound2-dev`)

## How to extend
This README will be updated as new features are added (e.g., tempo, musical figures, MIDI support, etc).
