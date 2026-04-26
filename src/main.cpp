#include "AudioEngine.hpp"
#include <chrono>
#include <thread>
#include <raylib.h>
#include <cmath>

int main()
{
	RingBuffer buffer;
	AudioEngine au(buffer);
	au.setModulationIndex(150);
	au.setModulationRatio(1);
	
	InitWindow(800, 600, "FM Synthesizer");
	SetTargetFPS(30);

	float rotationOffset = 0.0f;

	while (!WindowShouldClose())
	{
		float    samples[88200 / 30];
		int      numSamples = buffer.read(samples, 88200 / 30);

		BeginDrawing();
		ClearBackground(BLACK);

		if (numSamples > 1) // Aseguramos que hay datos
		{
			// --- 1. ZERO-CROSSING (Sincronización) ---
			int startIndex = 0;
			// Buscamos un cruce de negativo a positivo en la primera parte del buffer
			for (int i = 0; i < numSamples / 4; i++) {
				if (samples[i] <= 0.0f && samples[i+1] > 0.0f) {
					startIndex = i;
					break;
				}
			}

			// Descartamos las muestras previas al cruce por cero
			int samplesToDraw = numSamples - startIndex;
			
			int centerX = GetScreenWidth() / 2;
			int centerY = GetScreenHeight() / 2;
			int lastX = -1;
			int lastY = -1;
			
			// Reducimos la velocidad base de rotación para que sea más relajante
			rotationOffset += 0.0; 

			for (int i = 0; i < samplesToDraw; i++)
			{
				// --- 2. HANN WINDOW (Suavizado de la cicatriz) ---
				// Calculamos el valor de la ventana para esta posición
				float window = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (samplesToDraw - 1)));
				
				// Multiplicamos la muestra por la ventana
				float smoothedSample = samples[startIndex + i] * window;

				// El radio ahora usa la muestra suavizada
				float radius = 150.0f + (smoothedSample * 250.0f); // Subí el multiplicador para más impacto
				
				// Repartimos el ángulo entre las muestras útiles
				float angle = rotationOffset + ((float)i / (float)samplesToDraw * 2.0f * M_PI);
				
				int x = centerX + (int)(std::sin(angle) * radius);
				int y = centerY + (int)(std::cos(angle) * radius);
				
				// El color también usa la muestra suavizada
				float intensity = std::abs(smoothedSample);
				if (intensity > 1.0f) intensity = 1.0f;

				unsigned char r = (unsigned char)(138 + intensity * (255 - 138));
				unsigned char g = (unsigned char)(43  + intensity * (255 - 43));
				unsigned char b = (unsigned char)(226 + intensity * (0   - 226));
				Color pointColor = { r, g, b, 255 };

				if (lastX != -1 && lastY != -1)
					DrawLine(lastX, lastY, x, y, pointColor);
				else
					DrawPixel(x, y, pointColor);
					
				lastX = x;
				lastY = y;
			}

			// Al aplicar la ventana Hann, el primer y último punto de deformación siempre es 0.
			// Por lo tanto, cerramos el círculo en el radio base exacto.
			float firstRadius = 150.0f; 
			float firstAngle = rotationOffset;
			int firstX = centerX + (int)(std::sin(firstAngle) * firstRadius);
			int firstY = centerY + (int)(std::cos(firstAngle) * firstRadius);
			DrawLine(lastX, lastY, firstX, firstY, WHITE);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
