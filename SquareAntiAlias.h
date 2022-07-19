/* SquareAntiAlias.h: header for square waveform generator class with reduced aliasing
 * Sara Adkins
 */
#pragma once

#include <vector>
#include "SawAntiAlias.h"

class SquareAntiAlias {
public:
	SquareAntiAlias() {} // Default constructor
	SquareAntiAlias(float sampleRate); 						
	
	void setup(float sampleRate); 		
	
	void setFrequency(float f);	// Set the oscillator frequency
	float getFrequency(); // Get the oscillator frequency
	
	float process(); // Get the next sample and update the phase
	
	~SquareAntiAlias() {} // Destructor

private:
	//square wave made by summing phase-offset sawtooths
	SawAntiAlias saw1_;
	SawAntiAlias saw2_;
};