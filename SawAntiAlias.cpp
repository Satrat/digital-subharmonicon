/* SawAntiAlias.cpp: implements sawtooth waveform generator class with reduced aliasing
 * Sara Adkins
 */
#include <cmath>
#include "SawAntiAlias.h"

SawAntiAlias::SawAntiAlias(float sampleRate, float phaseOffset) {
	setup(sampleRate, phaseOffset);
} 

void SawAntiAlias::setup(float sampleRate, float phaseOffset)
{
	// It's faster to multiply than to divide on most platforms, so we save the inverse
	// of the sample rate for use in the phase calculation later
	inverseSampleRate_ = 1.0 / sampleRate;
	
	// used to scale output waveform based on f0
	scalingFactor_ = sampleRate / 4.0f;
	
	// Initialise the starting state
	phase_ = phaseOffset;
	z1_ = 1.0f;
}

// Set the oscillator frequency
void SawAntiAlias::setFrequency(float f) {
	frequency_ = f;
}

// Get the oscillator frequency
float SawAntiAlias::getFrequency() {
	return frequency_;
}			
	
// Get the next sample and update the phase
float SawAntiAlias::process() {
	float out = 0;
	
	//Algorithm from Valimaki 2006
	float bphase = phase_ * 2.0f - 1.0f; //convert [0,1] ramp to [-1,1]
	float sqr_bphase = bphase * bphase; //turn sawtooth into parabolic waveform
	out = (sqr_bphase  - z1_); //differentiate
	z1_ = sqr_bphase; //save state for next differentiation
	out = out * scalingFactor_ / frequency_; //recover original amplitude by scaling by f0
	
	// update and wrap phase
	phase_ = phase_ + (inverseSampleRate_ * frequency_);
	while(phase_ > 1.0f) {
		phase_ -= 1.0f;
	}
	
	return out;
}	