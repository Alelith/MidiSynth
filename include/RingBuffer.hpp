#ifndef RINGBUFFER_HPP
# define RINGBUFFER_HPP

# include <atomic>

using std::atomic;

class RingBuffer
{
	public:
		RingBuffer() = default;
		~RingBuffer() = default;

		int	write(const float *data, int numSamples);
		int	read(float *data, int numSamples);
	private:
		float		buffer[16384];
		atomic<int>	occupancy = 0;
		atomic<int>	writePos = 0;
		atomic<int>	readPos = 0;
};

#endif /* RINGBUFFER_HPP */
