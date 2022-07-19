// FOFilter.h: header file for first order filter class
// Copied from Assigment1 submission
// Sara Adkins

#pragma once

#include <vector>

class FOFilter {
public:
	FOFilter(); // Default constructor
	FOFilter(float sampleRate, float frequencyHz, float resonance); // Constructor with arguments to initialize filter
	
	void calculate_coefficients(float sampleRate, float frequencyHz, float resonance); // Set parameters
	
	float process(float in); // Get the next sample and update state variables
	
	float getY1(); // return y[n-1]
	
	float getGRes(); // return filter resonance
	
	~FOFilter() {} // Destructor

private:
	float B0_;
	float B1_;
	float A1_;
	
	float GRes_;
	
	float X1_;
	float Y1_;
};
