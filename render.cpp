// Queen Mary University of London
// ECS7012 - Music and Audio Programming
// Spring 2022
//
// Final Project: Moog Subharmonicon Replica
// Sara Adkins

#include <Bela.h>
#include <libraries/Gui/Gui.h>
#include <libraries/Scope/Scope.h>
#include "Oscillator.h"
#include "ResFilter.h"
#include "Debouncer.h"
#include "ASR.h"
#include "Sequence.h"

// Browser-based GUI to adjust parameters
Gui gGui;

// Browser-based oscilloscope to visualise signal
Scope gScope;

// Two oscillators with shared 4th order Moog filter
Oscillator gOsc1, gOsc2;
ResFilter filter;

// Each oscillator can be modulated by a sequence
Sequence seq1, seq2;

//analog input pins
int gAudioFramesPerAnalogFrame;
const int kOsc1FreqChannel = 0;
const int kCutoffChannel = 1;
const int kResChannel = 2;
const int kVolumeChannel = 3;
const int kTempoChannel = 4;
const int kOsc2FreqChannel = 5;
const int kOsc1AmpChannel = 6;
const int kOsc2AmpChannel = 7;
const int kGatePin = 0;
const int kPlayPin = 1;

// constants as defined in subharmonicon manual
const float kMinCutoffFreq = 20.0f;
const float kMaxCutoffFreq = 20000.0f;
const float kMinAttack = 0.001;
const float kMaxAttack = 2.0; // altered from moog 10
const float kMinDecay = 0.005;
const float kMaxDecay = 2.0; //altered from moog 10
const float kMinEg = -10000.0f;
const float kMaxEg = 10000.0f;
const float kMinTempo = 0.333f;
const float kMaxTempo = 300.0f; // altered this from moog 3000


Debouncer gDebouncerGate, gDebouncerPlay; //button debouncer
ASR gAmplitudeASR, gFilterASR; //envelopes for amplitude and filter cutoff

//synthesizer play states
enum PlayState {
	OFF = 0, // silent
	GATED = 1, //trigger on button press and release
	SEQUENCE = 2 //trigger on sequence ticks
};
PlayState state_ = OFF;

//keep track of base tempo
int gMetroPeriod;
int gMetroCounter = 0;

//keep track of rhythms, always a multiple of base tempo
std::vector<int> gRhythmDivs;
std::vector<int> gRhythmCounters;
const int kNumRhythms = 4;

//which sequence a rhythm should trigger
enum RhythmTarget {
	NO_TARGET = 0,
	SEQ1 = 1,
	SEQ2 = 2,
	BOTH = 3
};
std::vector<RhythmTarget> gRhythmTargets;

//buffer offsets for parameters received from GUI
const int osc1Offset = 0; //3
const int osc2Offset = 3; //3
const int subOscOffset = 6; //4
const int scaleOffset = 10; //1
const int envelopeParamsOffset = 11; //5
const int envelopeEgOffset = 16; //1
const int seq1BeatOffset = 17; //4
const int seq2BeatOffset = 21; //4
const int seqModeOffset = 25; //2
const int seqRangeOffset = 27; //1
const int rhythmTargetsOffset = 28; //4
const int rhythmDivsOffset = 32; //4
const int kNumGuiParams = 36;

bool setup(BelaContext *context, void *userData)
{
	//Ensure analog channels are enabled
	if(context->analogFrames == 0 ) {
		rt_printf("Error: this example needs analog enabled\n");
		return false;
	}
	gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
		
	//setup oscillators and their sequences
	gOsc1.setup(context->audioSampleRate, SAW);
	gOsc2.setup(context->audioSampleRate, SAW);
	//rt_printf("INIT BOTH OSC\n");
	filter.setup(context->audioSampleRate, 4);
	seq1.setup();
	seq2.setup();

	// Set up the GUI
	gGui.setup(context->projectName);
	gGui.setBuffer('f', kNumGuiParams);
	
	//setup digital input to read buttons with a debounce of 50ms
	pinMode(context, 0, kGatePin, INPUT); //set input
	gDebouncerGate.setup(context->audioSampleRate, .05);
	pinMode(context, 0, kPlayPin, INPUT);
	gDebouncerPlay.setup(context->audioSampleRate, .05);
	
	//setup envelopes
	gAmplitudeASR.setSampleRate(context->audioSampleRate);
	gFilterASR.setSampleRate(context->audioSampleRate);
	
	//initialize base tempo to 60BPM
	gMetroPeriod = (int)context->audioSampleRate * 1.0f;
	gMetroCounter = gMetroPeriod; // so we respond immediately on play

	//initialze all rhythms as matching base tempo
	//turn on rhythm1 to trigger sequence 1 as default state
	gRhythmDivs.resize(kNumRhythms);
	gRhythmCounters.resize(kNumRhythms);
	gRhythmTargets.resize(kNumRhythms);
	for(unsigned int i = 0; i < kNumRhythms; i++) {
		gRhythmDivs[i] = 1;
		gRhythmCounters[i] = 0.0f;
		gRhythmTargets[i] = NO_TARGET;
	}
	gRhythmTargets[0] = SEQ1;
	
	// Set up the scope
	gScope.setup(1, context->audioSampleRate);

	return true;
}

//read envelope parameters from GUI buffer once per audio block
void setEnvelopeParams(float *data)
{
	float ampAtk = map(data[0], 0.0, 1.0, kMinAttack, kMaxAttack);
	float ampDec = map(data[1], 0.0, 1.0, kMinDecay, kMaxDecay);
	float freqAtk = map(data[2], 0.0, 1.0, kMinAttack, kMaxAttack);
	float freqDec = map(data[3], 0.0, 1.0, kMinDecay, kMaxDecay);
	gAmplitudeASR.setAttackTime(ampAtk);
	gAmplitudeASR.setReleaseTime(ampDec);
	gFilterASR.setAttackTime(freqAtk);
	gFilterASR.setReleaseTime(freqDec);
}

//read sequence frequency offsets from GUI buffer once per audio block
void setSeqBeats(Sequence *seq, float *data)
{
	seq->setBeatOffset(0, data[0]);
	seq->setBeatOffset(1, data[1]);
	seq->setBeatOffset(2, data[2]);
	seq->setBeatOffset(3, data[3]);
}

//read rhythm targets from GUI buffer once per audio block
void setRhythmTargets(float *data)
{
	gRhythmTargets[0] = (RhythmTarget)(int)data[0];
	gRhythmTargets[1] = (RhythmTarget)(int)data[1];
	gRhythmTargets[2] = (RhythmTarget)(int)data[2];
	gRhythmTargets[3] = (RhythmTarget)(int)data[3];
}

//read rhythm tempo multiples from GUI buffer once per audio block
void setRhythmDivs(float *data)
{
	gRhythmDivs[0] = data[0];
	gRhythmDivs[1] = data[1];
	gRhythmDivs[2] = data[2];
	gRhythmDivs[3] = data[3];
}

//read oscillator parameters from GUI buffer once per audio block
void setOscParams(Oscillator *osc, float *data)
{
	osc->setWaveType((WaveType)(int)data[0]);	
	osc->setSub1Ratio(data[1]);
	osc->setSub2Ratio(data[2]);
}

void render(BelaContext *context, void *userData)
{
	//rt_printf("RENDER\n");
	
	//parse GUI parameters
	DataBuffer& buffer = gGui.getDataBuffer(0);
	float* data = buffer.getAsFloat();
	
	setOscParams(&gOsc1, data + osc1Offset);
	setOscParams(&gOsc2, data + osc2Offset);
	float subOsc1Amp = data[subOscOffset];
	float subOsc2Amp = data[subOscOffset+1];
	float subOsc1Amp2 = data[subOscOffset+2];
	float subOsc2Amp2 = data[subOscOffset+3];
	Scale scale = (Scale)(int)(data[scaleOffset]);
	gOsc1.setScale(scale);
	gOsc2.setScale(scale);

	setEnvelopeParams(data + envelopeParamsOffset);
	float eg = map(data[envelopeEgOffset], -1.0, 1.0, kMinEg, kMaxEg);
	
	setSeqBeats(&seq1, data + seq1BeatOffset);
	setSeqBeats(&seq2, data + seq2BeatOffset);
	SeqMode mode = (SeqMode)(int)(data[seqModeOffset]);
	SeqMode mode2 = (SeqMode)(int)(data[seqModeOffset + 1]);
	seq1.setRange((int)data[seqRangeOffset]);
	seq2.setRange((int)data[seqRangeOffset]);
	
	setRhythmDivs(data + rhythmDivsOffset);
	setRhythmTargets(data + rhythmTargetsOffset);
	
	//rt_printf("RENDER PARAMS DONE\n");
    for(unsigned int n = 0; n < context->audioFrames; n++) {
		//update base tempo(corresponds to sub-beat period)
		float periodHz = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kTempoChannel), 0, 3.3/4.096, kMinTempo, kMaxTempo);
		gMetroPeriod = (1.0f / periodHz) * context->audioSampleRate;
		
		//update oscillator frequency, volume ane wave type. Subharmonics are updated accordingly
		float oscAmplitude = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kOsc1AmpChannel), 0, 3.34/4.096, 0.0f, 1.0f);
		float osc2Amplitude = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kOsc2AmpChannel), 0, 3.34/4.096, 0.0f, 1.0f);
    	float oscFrequency = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kOsc1FreqChannel), 0, 3.3/4.096, kMinVcoFreq, kMaxVcoFreq);
    	float oscFrequency2 = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kOsc2FreqChannel), 0, 3.3/4.096, kMinVcoFreq, kMaxVcoFreq);
    	seq1.modulateOscillator(oscFrequency, mode, &gOsc1);
    	seq2.modulateOscillator(oscFrequency2, mode2, &gOsc2);
    	
    	//update filter parameters
    	float cutoff = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kCutoffChannel), 0, 3.34/4.096, kMinCutoffFreq, kMaxCutoffFreq);
    	float resonance = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kResChannel), 0, 3.34/4.096, 0.0, 1.0);
    	float volume = map(analogRead(context, n/gAudioFramesPerAnalogFrame, kVolumeChannel), 0, 3.34/4.096, 0.0f, 1.0f); //update global volume
    	
    	//calculate start and endpoint of cutoff envelope, clipping to valid range
    	float start, rampAmnt;
    	if(cutoff - eg < kMinCutoffFreq) {
    		start = kMinCutoffFreq;
    		rampAmnt = (cutoff - start);
    	}
    	else if(cutoff - eg > kMaxCutoffFreq) {
    		start = kMaxCutoffFreq;
    		rampAmnt = (cutoff - start);
    	}
    	else {
    		start = cutoff - eg;
    		rampAmnt = eg;
    	}
    	
    	//process any changes in play state based on button status
    	int gateValue = digitalRead(context, n, kGatePin);
    	gDebouncerGate.process(gateValue);
    	int playValue = digitalRead(context, n, kPlayPin);
    	gDebouncerPlay.process(playValue);
    	if(state_ == OFF) {
    		if(gDebouncerPlay.fallingEdge()) { //start sequence button pressed
    			state_ = SEQUENCE;
    			gAmplitudeASR.setSustainMode(false); // go immediately from attack to decay in sequence mode
    			gFilterASR.setSustainMode(false);
    			seq1.reset();
    			seq2.reset();
    			gMetroCounter = gMetroPeriod; //so sequence starts up immediately
    		}
    		else if(gDebouncerGate.fallingEdge()){ //trigger button pressed
    			state_ = GATED;
    			gAmplitudeASR.trigger();
	    		gFilterASR.trigger();
    		}
    	}
    	else if(state_ == GATED) {
    		if(gDebouncerGate.fallingEdge()) { //untrigger button pressed
    			state_ = OFF;
    			gAmplitudeASR.release();
	    		gFilterASR.release();
    		}
    	}
    	else if(state_ == SEQUENCE) {
    		if(gDebouncerPlay.fallingEdge()) { //stop sequence button pressed
    			state_ = OFF;
    			gAmplitudeASR.release();
    			gFilterASR.release();
    			gAmplitudeASR.setSustainMode(true);
    			gFilterASR.setSustainMode(true);
    		}
    		else if(++gMetroCounter >= gMetroPeriod) { //new beat reached in sequence
    			seq1.setIsActive(false); //by default sequence is inactive unless triggered by a rhythm
    			seq2.setIsActive(false);
    			for(unsigned int i = 0; i < kNumRhythms; i++) { //loop through rhythm array
    				if(gRhythmTargets[i] == SEQ1) {
    					seq1.setIsActive(true);
    					if(++(gRhythmCounters[i]) >= gRhythmDivs[i]) { //update progress towards super-beat
    						//if we're on a super-beat move sequence forward and trigger envelope
    						seq1.beat();
			    			gAmplitudeASR.trigger();
			    			gFilterASR.trigger();
			    			gRhythmCounters[i] = 0;
    					}
    				}
    				else if(gRhythmTargets[i] == SEQ2) {
    					seq2.setIsActive(true);
    					if(++(gRhythmCounters[i]) >= gRhythmDivs[i]) {
			    			seq2.beat();
			    			gAmplitudeASR.trigger();
			    			gFilterASR.trigger();
			    			gRhythmCounters[i] = 0;
    					}
    				}
    				else if(gRhythmTargets[i] == BOTH) {
    					seq1.setIsActive(true);
    					seq2.setIsActive(true);
    					if(++(gRhythmCounters[i]) >= gRhythmDivs[i]) {
    						seq1.beat();
			    			seq2.beat();
			    			gAmplitudeASR.trigger();
			    			gFilterASR.trigger();
			    			gRhythmCounters[i] = 0;
    					}
    				}
    			}
    			gMetroCounter = 0; // reset sub-beat counter
    		}
    	}
    	
		// Get the next value from the ASR envelopes
    	float amplitude = gAmplitudeASR.process();
    	float filterRamp = gFilterASR.process();
    	
    	filter.updateSections(context->audioSampleRate, start + filterRamp * rampAmnt, resonance); //update resonant filter
    	
		// combine samples from each active oscillator and apply filter
    	float out = 0.0f;
    	float outOsc1 = gOsc1.process(oscAmplitude, subOsc1Amp, subOsc2Amp);
    	float outOsc2 = gOsc2.process(osc2Amplitude, subOsc1Amp2, subOsc2Amp2);
    	if(state_ == SEQUENCE) {
    		if(seq1.getIsActive()) out += outOsc1;
    		if(seq2.getIsActive()) out += outOsc2;
    	} else {
    		out += outOsc1;
    		out += outOsc2;
    	}
    	out = filter.process(out) * amplitude * volume;
    	
        // Write the output to every audio channel
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
    		audioWrite(context, n, channel, out);
    	}
    	
    	gScope.log(out);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}
