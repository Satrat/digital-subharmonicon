/* SawAntiAlias.cpp: implements sawtooth waveform generator class with reduced aliasing
 * Sara Adkins
 */
#include <cmath>
#include "SquareAntiAlias.h"
#include <Bela.h>

SquareAntiAlias::SquareAntiAlias(float sampleRate) {
	setup(sampleRate);
} 

void SquareAntiAlias::setup(float sampleRate)
{
	//sawtooth oscillators offset by 50% combine to square wave
	saw1_.setup(sampleRate, 0.0f);
	saw2_.setup(sampleRate, 0.5f);
}

// Set the oscillator frequency
void SquareAntiAlias::setFrequency(float f) {
	saw1_.setFrequency(f);
	saw2_.setFrequency(f);
}

// Get the oscillator frequency
float SquareAntiAlias::getFrequency() {
	return saw1_.getFrequency();
}			
	
// Get the next sample and update the phase
float SquareAntiAlias::process() {
	//algorithm from Valimaki 2006
	return saw1_.process() - saw2_.process();
}	