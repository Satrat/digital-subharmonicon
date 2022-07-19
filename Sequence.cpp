/* Sequence.cpp: implements sequence class of oscillator modulations
 * Sara Adkins
 */
#include "Sequence.h"
#include "Oscillator.h"
#include <cmath>

void Sequence::setup() {
	currRange_ = ranges_[0];
	metroBeat_ = 0;
	beatOffsets_.resize(kNumBeats_);
	for(unsigned int i = 0; i < kNumBeats_; i++) {
		beatOffsets_[i] = 0.0f;
	}
	isActive_ = false;
}

void Sequence::setRange(int rangeIdx) {
	currRange_ = ranges_[rangeIdx];
}

//set offset of current beat, normalized [-1,1]
void Sequence::setBeatOffset(int beatIdx, float value) {
	beatOffsets_[beatIdx] = value;
}

//how many octaves to offset base frequency on current beat
float Sequence::getCurrOffset() {
	return powf(2.0, currRange_ * beatOffsets_[metroBeat_]);
}

void Sequence::setIsActive(bool isActive) {
	isActive_ = isActive;
}

bool Sequence::getIsActive() {
	return isActive_;
}

void Sequence::modulateOscillator(float oscFrequency, SeqMode mode, Oscillator *osc) {
	//determine which waveform of oscillator to modulate
	if(mode == VCO) {
		oscFrequency = oscFrequency * getCurrOffset(); //get modulated frequency
		oscFrequency = std::max(std::min(kMaxOscFreq, oscFrequency), kMinOscFreq); //clip to valid range
		osc->setFrequency(oscFrequency, 0, 0); //set vco frequency with no change to subharmonic ratio
	}
	else if(mode == SUB1) {
		int octOffset = round(currRange_ * beatOffsets_[metroBeat_]);
		osc->setFrequency(oscFrequency, octOffset, 0); // modulate ratio of subharmonic 1
	}
	else { //SUB2
		int octOffset = round(currRange_ * beatOffsets_[metroBeat_]);
		osc->setFrequency(oscFrequency, 0, octOffset); // modulate ratio of subharmonic 2
	}
}

void Sequence::beat() {
	// progress to next beat in sequence, wrap around
	if(++metroBeat_ >= kNumBeats_) {
		metroBeat_ = 0;
	}
}

void Sequence::reset() {
	// reset to beat 1
	metroBeat_ = 0;
}