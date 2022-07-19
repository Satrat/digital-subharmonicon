/* SawAntiAlias.h: header for sawtooth waveform generator class with reduced aliasing
 * Sara Adkins
 */
#pragma once

#include <vector>

class SawAntiAlias {
public:
	SawAntiAlias() {} // Default constructor
	SawAntiAlias(float sampleRate, float phaseOffset); 						
	
	void setup(float sampleRate, float phaseOffset); 		
	
	void setFrequency(float f);	// Set the oscillator frequency
	float getFrequency(); // Get the oscillator frequency
	
	float process(); // Get the next sample and update the phase
	
	~SawAntiAlias() {} // Destructor

private:
	float phase_; // current phase
	float z1_; // store previous sample

	float inverseSampleRate_; // 1 divided by the audio sample rate	
	float scalingFactor_; //f0-based scaling
	float frequency_; // Frequency of the oscillator
};