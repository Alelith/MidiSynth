/**
 * @file RingBuffer.hpp
 * @brief Thread-safe circular buffer for audio data
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#ifndef RINGBUFFER_HPP
# define RINGBUFFER_HPP

# include <atomic>

/**
 * @defgroup ringbuffer_module Lock-Free Ring Buffer
 * @brief Wait-free circular data structure for cross-thread audio transfer
 * 
 * @details This module provides a fixed-size lock-free buffer mechanism
 * designed to safely exchange floating-point audio data between the
 * generator thread and the OS-level PortAudio callback thread.
 * 
 * @section module_features_sec Features
 * - Fixed 16384 sample internal capacity
 * - Lock-free multi-threading operations via std::atomic
 * - Safe concurrent read and write operations
 * 
 * @section module_usage_sec Usage
 * Instantiate RingBuffer to share between AudioEngine threads. Use write()
 * on the synthesis side to push data, and read() within the callback routine.
 */

using std::atomic;

/**
 * @class RingBuffer
 * @brief Concurrent circular queue for PCM audio data
 * 
 * The RingBuffer class provides thread-safe operations pushing and pulling
 * audio frames across separated threads without utilizing blocking mutexes.
 */
class RingBuffer
{
	public:
		RingBuffer() = default;
		~RingBuffer() = default;

		int	write(const float *data, int numSamples);
		int	read(float *data, int numSamples);
	private:
		float		buffer[16384];   ///< Fixed-size static floating-point memory array
		atomic<int>	occupancy = 0;   ///< Thread-safe count of unread available samples
		atomic<int>	writePos = 0;    ///< Thread-safe current writing index 
		atomic<int>	readPos = 0;     ///< Thread-safe current reading index
};

#endif /* RINGBUFFER_HPP */
