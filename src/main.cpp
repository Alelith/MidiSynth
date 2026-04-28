#include "AudioEngine.hpp"
#include <chrono>
#include <thread>
#include <raylib.h>
#include <cmath>
#include <algorithm>
#include <fftw3.h>

int main()
{
	RingBuffer	buffer;
	AudioEngine	au(buffer);
	
	au.setModulationIndex(20);
	au.setModulationRatio(2);
	
	InitWindow(1920, 1080, "FM Synthesizer - FFTW Mandala");
	SetTargetFPS(60);

	const int	FFT_SIZE = 1024;
	float		tempBuffer[16384];
	float		historyBuffer[FFT_SIZE] = {0.0f};

	// --- 1. CONFIGURACIÓN DE FFTW (Precisión Simple 'float') ---
	// Reservamos memoria para el input (tiempo) y el output (frecuencia)
	float	*fft_in = (float*) fftwf_malloc(sizeof(float) * FFT_SIZE);
	
	// Una FFT de una señal real genera (N/2 + 1) números complejos
	int		numBins = (FFT_SIZE / 2) + 1; 
	fftwf_complex	*fft_out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * numBins);
	
	// Creamos el plan. FFTW_ESTIMATE es ideal aquí porque analiza y arranca rápido.
	fftwf_plan plan = fftwf_plan_dft_r2c_1d(FFT_SIZE, fft_in, fft_out, FFTW_ESTIMATE);

	float	magnitudes[1024]; 
	float	rotationOffset = 0.0f;

	while (!WindowShouldClose())
	{
		int		samplesRead = buffer.read(tempBuffer, 16384);
		
		// Añadir los nuevos samples al historial desplazando los antiguos
		if (samplesRead > 0)
		{
			int newSamples = std::min(samplesRead, FFT_SIZE);
			
			// Desplazar muestras antiguas hacia la izquierda
			for (int i = 0; i < FFT_SIZE - newSamples; i++)
				historyBuffer[i] = historyBuffer[i + newSamples];
				
			// Copiar las nuevas muestras al final
			for (int i = 0; i < newSamples; i++)
				historyBuffer[(FFT_SIZE - newSamples) + i] = tempBuffer[samplesRead - newSamples + i];
		}

		// --- 2. PREPARAR EL BUFFER (VENTANA DE HANN) ---
		for (int i = 0; i < FFT_SIZE; i++) 
		{
			float multiplier = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (FFT_SIZE - 1)));
			fft_in[i] = historyBuffer[i] * multiplier;
		}

		// --- 3. EJECUTAR LA MAGIA ---
		fftwf_execute(plan);

		// --- 4. CALCULAR LAS MAGNITUDES ---
		for (int i = 0; i < numBins; i++) 
		{
			float real = fft_out[i][0];
			float imag = fft_out[i][1];
			// Teorema de Pitágoras. Dividimos por (FFT_SIZE / 2) para normalizar la amplitud.
			magnitudes[i] = std::sqrt(real * real + imag * imag) / (FFT_SIZE / 2.0f);
		}

		// --- 5. DIBUJAR EL MANDALA ---
		rotationOffset += 0.002f; // El mandala girará lentamente sobre sí mismo
		
		BeginDrawing();
		ClearBackground(BLACK);

		int	centerX = GetScreenWidth() / 2;
		int	centerY = GetScreenHeight() / 2;
		
		float	baseRadius = 130.0f;
		float	maxRadius = 450.0f;
		
		// No mostramos todas las frecuencias porque las hiper-agudas suelen ser ruido blanco.
		// Con un cuarto de los bins (resolución baja-media) se ve mucho más limpio.
		int	displayBins = numBins / 4; 

		Vector2	prevPointRight = {0,0};
		Vector2	prevPointLeft = {0,0};

		for (int i = 1; i < displayBins; i++)
		{
			// Mapeamos el espectro: graves arriba (0), agudos hacia abajo (PI)
			float	fraction = (float)i / displayBins;
			float	theta = fraction * M_PI;
			
			// Multiplicamos exageradamente porque los armónicos de FM tienen amplitudes muy bajas comparadas con la fundamental
			//float	amp = magnitudes[i] * 400.0f;
			float	amp = std::pow(magnitudes[i], 0.5f) * 800.0f;
			//float	amp = std::log10(magnitudes[i] + 1.0f) * 500.0f;
			float	r = baseRadius + std::min(amp, maxRadius - baseRadius);

			// Simetría pura: Un lado suma el ángulo, el otro lo resta
			float	finalThetaRight = theta + rotationOffset;
			float 	finalThetaLeft  = -theta + rotationOffset;

			// Calculamos X e Y (usamos sin para X y cos para Y para que el 0 quede arriba)
			Vector2	currentPointRight = { centerX + r * std::sin(finalThetaRight), centerY - r * std::cos(finalThetaRight) };
			Vector2 currentPointLeft = { centerX + r * std::sin(finalThetaLeft), centerY - r * std::cos(finalThetaLeft) };

			if (i > 1)
			{
				// El color barre todo el arcoíris HSL (de rojo/graves a violeta/agudos)
				float	hue = fraction * 300.0f; 
				Color	lineColor = ColorFromHSV(hue, 0.9f, 0.9f);
				
				// Dibujamos el contorno de la onda en el mandala
				DrawLineEx(prevPointRight, currentPointRight, 3.0f, lineColor);
				DrawLineEx(prevPointLeft, currentPointLeft, 3.0f, lineColor);
				
				// Detalles visuales: Si una frecuencia salta con mucha fuerza, dibujamos un rayo translúcido desde el centro
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

	// --- 6. LIMPIEZA DE MEMORIA (¡Muy importante en C/C++!) ---
	fftwf_destroy_plan(plan);
	fftwf_free(fft_in);
	fftwf_free(fft_out);
	fftwf_cleanup();

	CloseWindow();
	return 0;
}
