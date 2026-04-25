#ifndef TUNINGSYS_HPP
# define TUNINGSYS_HPP

# include <vector>
# include <string>

using std::vector;
using std::string;

class TuningSys
{
	public:
		TuningSys();
		TuningSys(string tuningFile, float baseFreq = 432.0f, int midiNoteOffset = 69);
		~TuningSys();

		float	getFrequency(int note) const;
	private:
		vector<float>	notes;
		float			baseFrequency = 432.0f; // A4 frequency for standard tuning
		int				midiNoteOffset = 69; // MIDI note number for A4
};

#endif /* TUNINGSYS_HPP */
