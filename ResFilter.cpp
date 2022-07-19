/***** ResFilter.cpp *****/

#include "ResFilter.h"
#include <libraries/math_neon/math_neon.h>
#include <cmath>

ResFilter::ResFilter(float sampleRate, int filterOrder) {
	setup(sampleRate, filterOrder);
} 

void ResFilter::setup(float sampleRate, int filterOrder) {
	filterOrder_ = filterOrder;
	filters_.resize(filterOrder_);
	
	// Initialise each section of the 4th order filter
	for(unsigned int n = 0; n < filterOrder_; n++) {
		filters_[n].calculate_coefficients(sampleRate, 1000, 0.75);
	}
}

void ResFilter::updateSections(int sampleRate, float cutoff, float resonance) {
	// Calculate latest filter coefficients
	for(unsigned int n = 0; n < filterOrder_; n++) {
		filters_[n].calculate_coefficients(sampleRate, cutoff, resonance);
	}
}

float ResFilter::process(float in) {
	// apply the resonant filter to the input signal
	float Y1 = filters_[filterOrder_ - 1].getY1(); // get previous final output stored in last filter in bank
	float Gres = filters_[0].getGRes(); // get the resonance parameter from one of the filters
	float out = (1.0f + 4.0f * Gres * gComp_) * in - 4.0f * Gres * Y1; //calculate the feedback portion
	out = tanhf_neon(out); // nonlinearity
	for(unsigned int n = 0; n < filterOrder_; n++) { //cascading filters to create 4th order low pass
		out = filters_[n].process(out); //output of previous FOFilter is input to the next one
	}
	
	return out;
}