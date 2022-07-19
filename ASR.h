/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 14: ADSR
*/

/* ASR.h: header file for defining the ASR class
 * modified from original example code to remove Decay state and make Sustain optional
 */

#pragma once

#include "Ramp.h"

class ASR {
private:
	// ASR state machine variables, used internally
	enum State {
		StateOff = 0,
		StateAttack,
		StateSustain,
		StateRelease
	};

public:
	// Constructor
	ASR();
	
	// Constructor with argument
	ASR(float sampleRate);
	
	// Set the sample rate, used for all calculations
	void setSampleRate(float rate);
	
	// Start the envelope, going to the Attack state
	void trigger();
	
	// Stop the envelope, going to the Release state
	void release();
	
	// Calculate the next sample of output, changing the envelope
	// state as needed
	float process(); 
	
	// Indicate whether the envelope is active or not (i.e. in
	// anything other than the Off state
	bool isActive();
	
	// Methods for getting and setting parameters
	float getAttackTime() { return attackTime_; }
	float getSustainLevel() { return sustainLevel_; }
	float getReleaseTime() { return releaseTime_; }
	
	void setAttackTime(float attackTime);
	void setSustainLevel(float sustainLevel);
	void setReleaseTime(float releaseTime);
	void setSustainMode(bool doSustain);
	
	// Destructor
	~ASR();

private:
	// State variables and parameters, not accessible to the outside world
	float attackTime_;
	float sustainLevel_;
	float releaseTime_;
	
	bool doSustain_; //whether or not to skip sustain state
	
	State state_; // Current state of the ASR (one of the enum values above)
	Ramp ramp_;	// Line segment generator
};