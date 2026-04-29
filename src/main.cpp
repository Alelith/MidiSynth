/**
 * @file main.cpp
 * @brief Application entry point and visualizer implementation
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 * 
 * @details This file initializes the AudioEngine and sets up a Raylib
 * window for real-time audio visualization using FFTW for frequency analysis.
 * The visualizer creates a spinning, audio-reactive "mandala" showing
 * the harmonic content of the synthesized FM output.
 */

#include "AudioEngine.hpp"
#include <chrono>
#include <thread>
#include <raylib.h>
#include <cmath>
#include <algorithm>
#include <fftw3.h>

/**
 * @brief Main application loop
 * 
 * @details Sets up synchronization structures, audio engine, Window context,
 * and the FFTW plan. Continuously polls audio buffer to generate real-time
 * visual representations derived from frequency magnitudes.
 * 
 * @return OS exit status code
 * @retval 0 Execution finished successfully
 */
int main()
{
	// --- 1. AUDIO ENGINE INITIALIZATION ---
	// Create the shared lock-free buffer and start the AudioEngine thread
	RingBuffer	buffer;
	AudioEngine	au(buffer);
	
	// Configure initial FM synthesizer properties
	au.setModulationIndex(20);
	au.setModulationRatio(2);
	
	// --- 2. RAYLIB WINDOW INITIALIZATION ---
	InitWindow(1920, 1080, "FM Synthesizer - FFTW Mandala");
	SetTargetFPS(60);

	const int	FFT_SIZE = 1024;
	float		tempBuffer[16384];
	float		historyBuffer[FFT_SIZE] = {0.0f};

	// --- 3. FFTW CONFIGURATION (Single Precision 'float') ---
	// Allocate aligned memory for the time-domain input and frequency-domain output
	float	*fft_in = (float*) fftwf_malloc(sizeof(float) * FFT_SIZE);
	
	// A real-to-complex FFT generates (N/2 + 1) complex numbers
	int		numBins = (FFT_SIZE / 2) + 1; 
	fftwf_complex	*fft_out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * numBins);
	
	// Create the FFT execution plan. FFTW_ESTIMATE is used for fast initialization.
	fftwf_plan plan = fftwf_plan_dft_r2c_1d(FFT_SIZE, fft_in, fft_out, FFTW_ESTIMATE);

	float	magnitudes[1024]; 
	float	rotationOffset = 0.0f; // Controls the slow spinning effect of the mandala

	// --- 4. MAIN RENDER LOOP ---
	while (!WindowShouldClose())
	{
		// Pull freshly synthesized audio samples from the lock-free queue
		int		samplesRead = buffer.read(tempBuffer, 16384);
		
		if (samplesRead > 0)
		{
			// Determine how many new samples to integrate into the analysis window
			int newSamples = std::min(samplesRead, FFT_SIZE);
			
			// Shift old samples to the left to make room for new data
			for (int i = 0; i < FFT_SIZE - newSamples; i++)
				historyBuffer[i] = historyBuffer[i + newSamples];
				
			// Append the newest audio samples to the end of the history buffer
			for (int i = 0; i < newSamples; i++)
				historyBuffer[(FFT_SIZE - newSamples) + i] = tempBuffer[samplesRead - newSamples + i];
		}

		// --- 5. PREPARE BUFFER (HANN WINDOW) ---
		// Apply a Hann Window function to reduce spectral leakage at the edges
		for (int i = 0; i < FFT_SIZE; i++) 
		{
			float multiplier = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (FFT_SIZE - 1)));
			fft_in[i] = historyBuffer[i] * multiplier;
		}

		// --- 6. EXECUTE FAST FOURIER TRANSFORM ---
		// Transform the time-domain signal into its frequency components
		fftwf_execute(plan);

		// --- 7. CALCULATE FREQUENCY MAGNITUDES ---
		for (int i = 0; i < numBins; i++) 
		{
			float real = fft_out[i][0];
			float imag = fft_out[i][1];
			// Apply Pythagorean theorem to find the absolute amplitude, then normalize
			magnitudes[i] = std::sqrt(real * real + imag * imag) / (FFT_SIZE / 2.0f);
		}

		// --- 8. DRAW THE VISUAL MANDALA ---
		rotationOffset += 0.002f; // Increment rotation angle slightly each frame
		
		BeginDrawing();
		ClearBackground(BLACK);

		int	centerX = GetScreenWidth() / 2;
		int	centerY = GetScreenHeight() / 2;
		
		// Set visual radii limits for the drawing
		float	baseRadius = 130.0f;
		float	maxRadius = 450.0f;
		
		// Only display the lower/mid spectrum frequencies. High frequencies are
		// often visual noise in this context, so analyzing a quarter of bins is cleaner.
		int	displayBins = numBins / 4; 

		Vector2	prevPointRight = {0,0};
		Vector2	prevPointLeft = {0,0};

		// Traverse tracked frequency bins to plot visual arcs
		for (int i = 1; i < displayBins; i++)
		{
			// Map the spectrum linearly: Bass at top (0 radians), Treble at bottom (PI radians)
			float	fraction = (float)i / displayBins;
			float	theta = fraction * M_PI;
			
			// Exaggerate amplitudes visually. FM harmonics often have much lower
			// peaks compared to the fundamental carrier frequency.
			float	amp = std::pow(magnitudes[i], 0.5f) * 800.0f;
			
			// Limit the outward spike radius to avoid exceeding maxRadius bounds
			float	r = baseRadius + std::min(amp, maxRadius - baseRadius);

			// Pure symmetry: One arc maps clockwise (-theta), the other counter-clockwise (+theta)
			float	finalThetaRight = theta + rotationOffset;
			float 	finalThetaLeft  = -theta + rotationOffset;

			// Calculate cartesian coordinates. (Sin/Cos flipped so 0 sits at the top '12 o'clock')
			Vector2	currentPointRight = { centerX + r * std::sin(finalThetaRight), centerY - r * std::cos(finalThetaRight) };
			Vector2 currentPointLeft = { centerX + r * std::sin(finalThetaLeft), centerY - r * std::cos(finalThetaLeft) };

			if (i > 1)
			{
				// Map colors across the Hue spectrum (Red/Bass -> Violet/Treble)
				float	hue = fraction * 300.0f; 
				Color	lineColor = ColorFromHSV(hue, 0.9f, 0.9f);
				
				// Stroke the connecting line segments for both symmetrical sides
				DrawLineEx(prevPointRight, currentPointRight, 3.0f, lineColor);
				DrawLineEx(prevPointLeft, currentPointLeft, 3.0f, lineColor);
				
				// Visual flair: If a frequency bin spikes significantly, 
				// cast a translucent beam extending from the center outwards
				if (amp > 15.0f)
				{
					DrawLine(centerX, centerY, currentPointRight.x, currentPointRight.y, Fade(lineColor, 0.15f));
					DrawLine(centerX, centerY, currentPointLeft.x, currentPointLeft.y, Fade(lineColor, 0.15f));
				}
			}

			prevPointRight = currentPointRight;
			prevPointLeft = currentPointLeft;
		}

		EndDrawing();
	}

	// --- 9. MEMORY CLEANUP ---
	// Safely free all explicitly allocated FFTW structures to avoid memory leaks
	fftwf_destroy_plan(plan);
	fftwf_free(fft_in);
	fftwf_free(fft_out);
	fftwf_cleanup();

	CloseWindow();
	return 0;
}
