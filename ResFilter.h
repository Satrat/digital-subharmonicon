/* ResFilter.h: class for 4ith order low pass Moog filter
 * Adapted from Assignment 1 code
 * Sara Adkins
 */
#pragma once

#include <vector>
#include "FOFilter.h"

class ResFilter {
public:
	ResFilter() {}	// Default constructor
	ResFilter(float sampleRate, int filterOrder); 						
	
	void setup(float sampleRate, int filterOrder);
	
	void updateSections(int sampleRate, float cutoff, float resonance); //update coefficients to reflect new cutoff/resonance
	
	float process(float amplitude); //output next sample of filter output, update state

	~ResFilter() {}	// Destructor

private:
	// array for storing cascading filters
	int filterOrder_;
	std::vector<FOFilter> filters_;
	
	// fixed filter feedback parameter
	const float gComp_ = 0.5f;
};