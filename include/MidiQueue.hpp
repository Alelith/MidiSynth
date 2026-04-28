#ifndef MIDIQUEUE_HPP
# define MIDIQUEUE_HPP

# include <atomic>

using std::atomic;

struct MidiEvent
{
	int		note;
	float	frequency;
	bool	isOn;
	float	velocity;
};

class MidiQueue
{
	public:
		MidiQueue();

		bool	push(const MidiEvent& event);
		bool	pop(MidiEvent& event);
	private:
		MidiEvent	events[1024];
		atomic<int>	writeIndex;
		atomic<int>	readIndex;
};

#endif /* MIDIQUEUE_HPP */