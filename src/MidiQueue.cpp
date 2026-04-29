/**
 * @file MidiQueue.cpp
 * @brief Implementation of the circular lock-free MIDI queue
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-28
 */

#include "MidiQueue.hpp"

/**
 * @brief Default constructor for the queue
 * 
 * @details Ensures that internal atomic read and write index trackers
 * are properly initialized to zero, preventing array out-of-bounds accesses.
 * 
 * @ingroup midi_queue_module
 */
MidiQueue::MidiQueue() : writeIndex(0), readIndex(0) {}

/**
 * @brief Inserts a MIDI event into the queue
 * 
 * @details Employs lock-free memory barriers indicating immediate visibility
 * across different process handlers. Safely rejects data if buffer is exhausted.
 * 
 * @ingroup midi_queue_module
 * 
 * @param[in] event The specific structured MIDI playback command
 * @return Execution operation completion state
 * @retval true Pushed onto processing queue buffer correctly
 * @retval false Failed to insert due to maximum array occupancy
 */
bool MidiQueue::push(const MidiEvent& event)
{
	int currentWrite = writeIndex.load(std::memory_order_relaxed);
	int nextWrite = (currentWrite + 1) % 1024;

	// Check if the queue is full
	if (nextWrite == readIndex.load(std::memory_order_acquire))
		return false; // Queue is full

	events[currentWrite] = event;
	writeIndex.store(nextWrite, std::memory_order_release);
	return true;
}

/**
 * @brief Consumes a single pending MIDI event off the processing pipeline
 * 
 * @details Checks sequence trackers via memory barriers. Should pending
 * events exist, it extracts the oldest record while maintaining memory
 * synchronization guarantees logic.
 * 
 * @ingroup midi_queue_module
 * 
 * @param[out] event Pre-allocated configuration address taking results
 * @return Execution operation completion state
 * @retval true Retrieved processing element successfully
 * @retval false Queue is currently empty, parameter unchanged
 */
bool MidiQueue::pop(MidiEvent& event)
{
	int currentRead = readIndex.load(std::memory_order_relaxed);

	// Check if the queue is empty
	if (currentRead == writeIndex.load(std::memory_order_acquire))
		return false; // Queue is empty

	event = events[currentRead];
	readIndex.store((currentRead + 1) % 1024, std::memory_order_release);
	return true;
}
