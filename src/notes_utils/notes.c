#include "procedural_music.h"

/**
* @brief Simple hash function to convert a string to an unsigned int.
*
* @param str The input string to hash.

* @return The computed hash value.
*/
static unsigned int simple_hash(const char *str)
{
	unsigned int hash = 0;
	while (*str)
		hash += *str++;
	return hash;
}

t_note get_note(const char *str)
{
	if (!str)
		return -1;

	switch (simple_hash(str)) {
		case 67:  return C; break;	 // 'C'
		case 102: return Cs; break;  // 'C' + '#'
		case 136: return Db; break;  // 'D' + 'b'
		case 68:  return D; break;	// 'D'
		case 103: return Ds; break;  // 'D' + '#'
		case 138: return Eb; break;  // 'E' + 'b'
		case 69:  return E; break;	// 'E'
		case 70:  return F; break;	// 'F'
		case 105: return Fs; break;  // 'F' + '#'
		case 141: return Gb; break;  // 'G' + 'b'
		case 71:  return G; break;	// 'G'
		case 106: return Gs; break;  // 'G' + '#'
		case 163: return Ab; break;  // 'A' + 'b'
		case 65:  return A; break;	// 'A'
		case 100: return As; break;  // 'A' + '#'
		case 164: return Bb; break;  // 'B' + 'b'
		case 66:  return B; break;	// 'B'
	}
	return -1;
}
