// FOFilter.cpp: file for implementing the first order filter class
// Copied from Assignment 1 submission
// Sara Adkins

#include <cmath>
#include "FOFilter.h"
#include <cmath>

FOFilter::FOFilter() {
	B0_ = 0.0f;
	B1_ = 0.0f;
	A1_ = 0.0f;
	
	GRes_ = 0.0f;
	
	X1_ = 0.0f;
	Y1_ = 0.0f;
} 

FOFilter::FOFilter(float sampleRate, float frequencyHz, float resonance) {
	calculate_coefficients(sampleRate, frequencyHz, resonance);
	X1_ = 0.0f;
	Y1_ = 0.0f;
}
	
// Calculate filter coefficients given specifications
// frequencyHz -- filter frequency in Hertz (needs to be converted to discrete time frequency)
// resonance -- normalised parameter 0-1 which is related to filter Q
void FOFilter::calculate_coefficients(float sampleRate, float frequencyHz, float resonance) {
	float wc = 2.0f * M_PI * frequencyHz / sampleRate; // convert Hz to angular digital frequency
	
	// adjust cutoff to get desired measured cutoff frequency, polynomial approximation
	float g = 0.9892 * wc - 0.4342 * powf(wc, 2.0f) + 0.1381 * powf(wc, 3.0f) - 0.0202 * powf(wc, 4.0f);
	
	// calculate IIR coefficients
	B0_ = g * 1.0f / 1.3f;
	B1_ = g * 0.3f / 1.3f;
	A1_ = -(1.0f - g);
	
	// calculate resonance parameter using polynomial approximation
	GRes_ = resonance * (1.0029 + 0.0526 * wc - 0.0926 * powf(wc,2.0f) + 0.218 * powf(wc, 3.0f));
}
	
float FOFilter::process(float in) {
	//push one sample through IIR filter
	float out = B0_ * in + B1_ * X1_ - A1_ * Y1_;
	
	//remember states for next iteration
	X1_ = in;
	Y1_ = out;
	
	return out;
}

float FOFilter::getY1() {
	return Y1_;
}

float FOFilter::getGRes() {
	return GRes_;
}