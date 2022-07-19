/* Sequence.h: header for defining sequence of oscillator modulations
 * Sara Adkins
 */
#pragma once

#include <vector>
#include "Oscillator.h"

//Sequence can modulate one waveform of the oscillator
enum SeqMode {
	VCO = 0,
	SUB1 = 1,
	SUB2 = 2
};

//Oscillator output boundaries as defined in Moog manual
const float kMinOscFreq = 20.0f;
const float kMaxOscFreq = 10000.0f;

class Sequence {
public:
	Sequence() {} // Default constructor
	
	void setup();

	void setRange(int rangeIdx); // set octave range for sequence knobs
	
	void setBeatOffset(int beatIdx, float value); // set frequency offset at specified beat index
	float getCurrOffset(); // get current beat in sequence
	bool getIsActive(); // check if a rhythm is triggering this sequence
	void setIsActive(bool isActive); // indicate a rhythm is triggering this sequence
	void modulateOscillator(float oscFrequency, SeqMode mode, Oscillator *osc); // update oscillator frequency based on current beat settings
	
	void beat(); // increment beat position, wraps around
	void reset(); // reset to 1st beat
	
	~Sequence() {} // Destructor

private:
	const int kNumBeats_ = 4; // number of beats in sequence
	const std::vector<int> ranges_ = {1,2,5}; //frequency offset ranges, in +/- octaves
	
	int currRange_; //current frequency range
	int metroBeat_; //current beat
	std::vector<float> beatOffsets_; //keep track of frequency offsets at each beat position
	
	bool isActive_;

};