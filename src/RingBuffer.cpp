/**
 * @file RingBuffer.cpp
 * @brief Implementation of lock-free circular audio buffer operations
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#include "RingBuffer.hpp"
#include <cmath>

/**
 * @brief Pushes new floating-point frames into the buffer ring safely
 * 
 * @details Evaluates currently free occupancy inside the lock-free queue
 * then writes as many samples as possible avoiding thread race conditions.
 * 
 * @ingroup ringbuffer_module
 * 
 * @param[in] data Array containing scalar sound waves to load
 * @param[in] numSamples Expected data count frame insertion length
 * @return Safely inserted samples
 * @retval 0 If the internal buffer is functionally completely occupied
 * @retval \>0 Exact amount of frames accepted
 */
int	RingBuffer::write(const float *data, int numSamples)
{
	int	spaceAvailable = 16384 - occupancy.load();
	int	samplesToWrite = std::min(numSamples, spaceAvailable);
	for (int i = 0; i < samplesToWrite; i++)
	{
		buffer[writePos] = data[i];
		writePos = (writePos + 1) % 16384;
	}
	occupancy += samplesToWrite;
	return samplesToWrite;
}

/**
 * @brief Consumes available floating-point sample structures pulling from thread
 * 
 * @details Calculates the exact number of fully evaluated buffer sequences
 * returning the maximum requested up to what has already been provided implicitly.
 * 
 * @ingroup ringbuffer_module
 * 
 * @param[out] data Empty valid initialized memory segment array
 * @param[in] numSamples The desired exact buffer float length pull
 * @return Total valid data copied into output parameter segment array
 * @retval 0 No data available currently pending in queue
 * @retval \>0 Total scalar length of copied memory frames 
 */
int	RingBuffer::read(float *data, int numSamples)
{
	int	samplesAvailable = occupancy.load();
	int	samplesToRead = std::min(numSamples, samplesAvailable);
	for (int i = 0; i < samplesToRead; i++)
	{
		data[i] = buffer[readPos];
		readPos = (readPos + 1) % 16384;
	}
	occupancy -= samplesToRead;
	return samplesToRead;
}
