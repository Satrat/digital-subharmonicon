/* Oscillator.h: header for subharmonic oscillator class
 * Sara Adkins
 */
#pragma once

#include <vector>
#include "SawAntiAlias.h"
#include "SquareAntiAlias.h"

enum WaveType {
	SAW = 0,
	SQUARE = 1
};

enum Scale {
	NO_SCALE = 0,
	CHROMATIC = 1,
	MAJOR = 2,
	MINOR = 3,
	PENTATONIC = 4
};

// semi-tones above tonic for each scale type
const int kMajorDegrees[7] = {0,2,4,5,7,9,11};
const int kMajorLen = 7;
const int kMinorDegrees[7] = {0,2,3,5,6,8,10};
const int kMinorLen = 7;
const int kPentatonicDegrees[5] = {0,3,5,6,10};
const int kPentatonicLen = 5;

// oscillator boundaries as specific in Moog manual
const float kMinVcoFreq = 262.0f;
const float kMaxVcoFreq = 4186.0f;
const float kMinSubDiv = 1.0f;
const float kMaxSubDiv = 16.0f;

class Oscillator {
public:
	Oscillator() {}	// Default constructor
	Oscillator(float sampleRate, WaveType type); 						
	
	void setup(float sampleRate, WaveType type);
	
	void setFrequency(float frequency, float sub1Offset, float sub2Offset);
	void setWaveType(WaveType type);
	void setScale(Scale scale);
	
	// subharmonic oscillators set to base_freq / div_amnt Hz
	void setSub1Ratio(int div_amnt);
	void setSub2Ratio(int div_amnt);
	
	float getSub1Ratio() {return sub1DivAmnt_; }
	float setSub2Ratio() {return sub2DivAmnt_; }
	
	float process(float amplitude, float sub1Amp, float sub2Amp); //outout one sample of combined oscillators & update phase

	~Oscillator() {} // Destructor

private:
	WaveType waveType_;
	Scale scale_;
	
	SawAntiAlias sawtoothOscillator_;
	SawAntiAlias sawtoothSubOsc1_;
	SawAntiAlias sawtoothSubOsc2_;
	
	SquareAntiAlias squareOscillator_;
	SquareAntiAlias squareSubOsc1_;
	SquareAntiAlias squareSubOsc2_;
	
	//Get subharmonic frequencies by dividing base by these
	float sub1DivAmnt_;
	float sub2DivAmnt_;
};