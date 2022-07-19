/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 14: ADSR
*/

// ASR.cpp: implement the ASR (attack-sustain-release) class, modified from ADSR example

#include "ASR.h"

// Constructor. Set up some default parameters.
// We can also use initialisation lists before the 
// start of the curly braces to set these values
ASR::ASR()
{
	attackTime_ = 0.001;
	sustainLevel_ = 1;
	releaseTime_ = 0.001;

	doSustain_ = true;
	state_ = StateOff;
}

// Set the sample rate, used for all calculations
void ASR::setSampleRate(float rate) 
{
	ramp_.setSampleRate(rate);
}

// Start the envelope, going to the Attack state
void ASR::trigger() 
{
	if(state_ != StateAttack) { // Moog ignores triggers during attack phase
		state_ = StateAttack;
		ramp_.rampTo(sustainLevel_, attackTime_);
	}
}

// Stop the envelope, going to the Release state
void ASR::release() 
{
	// Go to the Release state from whichever state we were in
	state_ = StateRelease;
	ramp_.rampTo(0.0, releaseTime_);
}

// Calculate the next sample of output, changing the envelope
// state as needed
float ASR::process() 
{
	// Look at the state we're in to decide what value to return. 
	// This function handles the outputs within the state but
	// does not handle the transitions caused by external note events.
	// Those are done in trigger() and release().
	
   	if(state_ == StateOff) {
		// Nothing to do here. trigger() will change the state.
	}
	else if(state_ == StateAttack) {
		// Look for ramp to finish before moving to next phase
		if(ramp_.finished()) {
			if(doSustain_) state_ = StateSustain;
			else release();
		}
	}
	else if(state_ == StateSustain) {
		// Nothing to do here. release() will change the state.
	}
	else if(state_ == StateRelease) {
		// Wait until the envelope returns to 0
		if(ramp_.finished()) {
			state_ = StateOff;
		}
	}
    	
    // Return the current output level
    return ramp_.process();
}

// Indicate whether the envelope is active or not (i.e. in
// anything other than the Off state)
bool ASR::isActive() 
{
	return (state_ != StateOff);
}

// Methods to set the value of the parameters. We constrain
// each parameter to a sensible range
void ASR::setAttackTime(float attackTime)
{
	if(attackTime >= 0)
		attackTime_ = attackTime;
	else
		attackTime_ = 0;
}

void ASR::setSustainLevel(float sustainLevel)
{
	if(sustainLevel < 0)
		sustainLevel_ = 0;
	else if(sustainLevel > 1)
		sustainLevel_ = 1;
	else
		sustainLevel_ = sustainLevel;
}

void ASR::setReleaseTime(float releaseTime)
{
	if(releaseTime >= 0)
		releaseTime_ = releaseTime;
	else
		releaseTime_ = 0;
}

void ASR::setSustainMode(bool doSustain) {
	doSustain_ = doSustain;
}

// Destructor
ASR::~ASR() 
{
	// Nothing to do here
}