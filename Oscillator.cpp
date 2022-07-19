/* Oscillator.cpp: implements subharmonic oscillator class
 * Sara Adkins
 */

#include "Oscillator.h"
#include <cmath>

Oscillator::Oscillator(float sampleRate, WaveType type) {
	setup(sampleRate, type);
} 

void Oscillator::setup(float sampleRate, WaveType type) {
	waveType_ = type;
	scale_ = NO_SCALE;
	
	sawtoothOscillator_.setup(sampleRate, 0.0f);
	sawtoothSubOsc1_.setup(sampleRate, 0.0f);
	sawtoothSubOsc2_.setup(sampleRate, 0.0f);
	
	squareOscillator_.setup(sampleRate);
	squareSubOsc1_.setup(sampleRate);
	squareSubOsc2_.setup(sampleRate);
	
	
	// By default subharmonic oscillators are in unison with the main
	sub1DivAmnt_ = 1.0f;
	sub2DivAmnt_ = 1.0f;
}

void Oscillator::setFrequency(float frequency, float sub1Offset, float sub2Offset) {
	if(scale_ == NO_SCALE) {
		//do nothing, frequency stays the same
	}
	else if(scale_ == CHROMATIC) {
		//round frequency to nearest semitone
		int semitones = round(12.0f * log2(frequency / kMinVcoFreq));
		frequency = kMinVcoFreq * powf(2.0, semitones / 12.0f);
	}
	else{
		//calculate semitones past closest octave to get scale degrees
		//add 0.5 so next section rounds instead of truncates
		float semitones = 12.0f * log2(frequency / kMinVcoFreq);
		int trunc_semitones = (int)semitones;
		int numOctaves = trunc_semitones / 12;
		float pastOct = (semitones - trunc_semitones) + (trunc_semitones % 12) + 0.5f;
		
		//get information about currently selected scale
		int scaleLen;
		int* scaleDegrees;
		if(scale_ == MAJOR) {
			scaleLen = kMajorLen;
			scaleDegrees = (int *)kMajorDegrees;
		} else if(scale_ == MINOR) {
			scaleLen = kMinorLen;
			scaleDegrees = (int *)kMinorDegrees;
		} else { //PENTATONIC
			scaleLen = kPentatonicLen;
			scaleDegrees = (int *)kPentatonicDegrees;
		}
		
		//round to nearest scale degree
		//this isn't a perfect rounding, it biases down if scale degrees are further than a semitone
		for(unsigned int i = 0; i < scaleLen; i++) {
			if(pastOct == scaleDegrees[i]) {
				break; // already in scale
			}
			if(pastOct < scaleDegrees[i]) {
				pastOct = scaleDegrees[i];
				break;
			}
		}
		
		//convert back to Hz
		semitones = numOctaves * 12 + pastOct;
		frequency = kMinVcoFreq * powf(2.0, semitones / 12.0f);
	}
	
	//update subharmonic oscillators so they keep the correct ratio to the base
	float sub1Final = std::max(kMinSubDiv, std::min(kMaxSubDiv, (sub1DivAmnt_ + sub1Offset)));
	float sub2Final = std::max(kMinSubDiv, std::min(kMaxSubDiv, (sub2DivAmnt_ + sub2Offset)));
	
	//apply frequency update
	sawtoothOscillator_.setFrequency(frequency);
	sawtoothSubOsc1_.setFrequency(frequency / sub1Final);
	sawtoothSubOsc2_.setFrequency(frequency / sub2Final);
	
	squareOscillator_.setFrequency(frequency);
	squareSubOsc1_.setFrequency(frequency / sub1Final);
	squareSubOsc2_.setFrequency(frequency / sub2Final);
}

void Oscillator::setWaveType(WaveType type) {
	waveType_ = type;
}

void Oscillator::setScale(Scale scale) {
	scale_ = scale;
}

void Oscillator::setSub1Ratio(int div_amnt) {
	sub1DivAmnt_ = div_amnt;
	int currFreq = sawtoothOscillator_.getFrequency();
	sawtoothSubOsc1_.setFrequency(currFreq / sub1DivAmnt_);
	squareSubOsc1_.setFrequency(currFreq / sub1DivAmnt_);
}

void Oscillator::setSub2Ratio(int div_amnt) {
	sub2DivAmnt_ = div_amnt;
	int currFreq = sawtoothOscillator_.getFrequency();
	sawtoothSubOsc2_.setFrequency(currFreq / sub2DivAmnt_);
	squareSubOsc2_.setFrequency(currFreq / sub2DivAmnt_);
}

float Oscillator::process(float amplitude, float sub1Amp, float sub2Amp) {
	float out;
	//output scaled combined sample of whichever waveform is currently active
	if(waveType_ == SAW) {
		out = amplitude * sawtoothOscillator_.process() + sub1Amp * sawtoothSubOsc1_.process() + sub2Amp * sawtoothSubOsc2_.process();
	}
	else { //SQUARE
		out = amplitude * squareOscillator_.process() + sub1Amp * squareSubOsc1_.process() + sub2Amp * squareSubOsc2_.process();
	}
	return out;
}