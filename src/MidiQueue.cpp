#include "MidiQueue.hpp"

MidiQueue::MidiQueue() : writeIndex(0), readIndex(0) {}

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
