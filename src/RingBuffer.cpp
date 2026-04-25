#include "RingBuffer.hpp"
#include <cmath>

int	RingBuffer::write(const float *data, int numSamples)
{
	int	spaceAvailable = 4096 - occupancy.load();
	int	samplesToWrite = std::min(numSamples, spaceAvailable);
	for (int i = 0; i < samplesToWrite; i++)
	{
		buffer[writePos] = data[i];
		writePos = (writePos + 1) % 4096;
	}
	occupancy += samplesToWrite;
	return samplesToWrite;
}

int	RingBuffer::read(float *data, int numSamples)
{
	int	samplesAvailable = occupancy.load();
	int	samplesToRead = std::min(numSamples, samplesAvailable);
	for (int i = 0; i < samplesToRead; i++)
	{
		data[i] = buffer[readPos];
		readPos = (readPos + 1) % 4096;
	}
	occupancy -= samplesToRead;
	return samplesToRead;
}
