/* sketch.js: Javascript GUI for setting synth parameters. Passes buffer of parameters to audio application
 * Sara Adkins
 */

// buffer offsets, should match constants in render.cpp
const osc1Offset = 0; //3
const osc2Offset = 3; //3
const subOscOffset = 6; //4
const scaleOffset = 10; //1
const envelopeParamsOffset = 11; //5
const envelopeEgOffset = 16; //1
const seq1BeatOffset = 17; //4
const seq2BeatOffset = 21; //4
const seqModeOffset = 25; //2
const seqRangeOffset = 27; //1
const rhythmTargetsOffset = 28; //4
const rhythmDivsOffset = 32; //4

//buffer to send to Bela
let buffer = [0, 2, 3, 0, 2, 3, 0.2, 0.2, 0.0, 0.0, 0, 0.1, 0.1, 0.1, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1];

var wave_types = {
	'SAW': 0,
	'SQUARE': 1
};

var scale_types = {
	'None': 0,
	'CHROM': 1,
	'MAJ': 2,
	'MIN': 3,
	'PENT': 4
};

var range_types = {
	'+/-1 Oct': 0,
	'+/-2 Oct': 1,
	'+/-5 Oct': 2
};

var seq_mode_types = {
	'VCO': 0,
	'SUB1': 1,
	'SUB2': 2
};

var rhythm_types = {
	'NO_TARGET': 0,
	'SEQ1': 1,
	'SEQ2': 2,
	'BOTH': 3
};

let rhythms = [1, 0, 0, 0];

function setup() {
	subOsc1 = createSlider(1, 16, 2, 1);
	PsubOsc1 = createP("SUB 1 FREQ ");
	subOsc2 = createSlider(1, 16, 3, 1);
	PsubOsc2 = createP("SUB 2 FREQ ");
	
	subOsc1Amp = createSlider(0.0, 1.0, 0.2, 0);
	PsubOsc1Amp = createP("SUB 1 LEVEL ");
	subOsc2Amp = createSlider(0.0, 1.0, 0.2, 0);
	PsubOsc2Amp = createP("SUB 2 LEVEL ");
	
	sub2Osc1 = createSlider(1, 16, 2, 1);
	Psub2Osc1 = createP("SUB 1 FREQ ");
	sub2Osc2 = createSlider(1, 16, 3, 1);
	Psub2Osc2 = createP("SUB 2 FREQ ");
	
	sub2Osc1Amp = createSlider(0.0, 1.0, 0.0, 0);
	Psub2Osc1Amp = createP("SUB 1 LEVEL ");
	sub2Osc2Amp = createSlider(0.0, 1.0, 0.0, 0);
	Psub2Osc2Amp = createP("SUB 2 LEVEL ");
	
	ampAtk = createSlider(0.0, 1.0, 0.1, 0);
	PampAtk = createP("VCA ATTACK");
	ampDecay = createSlider(0.0, 1.0, 0.1, 0);
	PampDecay = createP("VCA DECAY");
	freqAtk = createSlider(0.0, 1.0, 0.1, 0);
	PfreqAtk = createP("VCF ATTACK");
	freqDecay = createSlider(0.0, 1.0, 0.1, 0);
	PfreqDecay = createP("VCF DECAY");
	freqEg = createSlider(-1.0, 1.0, 0.0, 0);
	PfreqEg = createP("VCF EG AMT");
	
	seq1Step1 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq1Step1 = createP("STEP 1");
	seq1Step2 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq1Step2 = createP("STEP 2");
	seq1Step3 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq1Step3 = createP("STEP 3");
	seq1Step4 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq1Step4 = createP("STEP 4");
	
	seq2Step1 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq2Step1 = createP("STEP 1");
	seq2Step2 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq2Step2 = createP("STEP 2");
	seq2Step3 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq2Step3 = createP("STEP 3");
	seq2Step4 = createSlider(-1.0, 1.0, 0.0, 0);
	Pseq2Step4 = createP("STEP 4");
	
	rhythm1 = createSlider(1, 16, 1, 1);
	Prhythm1 = createP("RHYTHM 1");
	rhythm2 = createSlider(1, 16, 1, 1);
	Prhythm2 = createP("RHYTHM 2");
	rhythm3 = createSlider(1, 16, 1, 1);
	Prhythm3 = createP("RHYTHM 3");
	rhythm4 = createSlider(1, 16, 1, 1);
	Prhythm4 = createP("RHYTHM 4");
	
	waveType = createRadio();
	waveType.option('SAW');
	waveType.option('SQUARE');
	waveType.selected('SAW');
	
	waveType2 = createRadio();
	waveType2.option('SAW');
	waveType2.option('SQUARE');
	waveType2.selected('SAW');
	
	scale = createRadio();
	Pscale = createP("QUANTIZE");
	scale.option('None');
	scale.option('CHROM');
	scale.option('MAJ');
	scale.option('MIN');
	scale.option('PENT');
	scale.selected('None');
	
	seqRange = createRadio();
	PseqRange = createP("SEQ OCT");
	seqRange.option('+/-1 Oct');
	seqRange.option('+/-2 Oct');
	seqRange.option('+/-5 Oct');
	seqRange.selected('+/-1 Oct');
	
	seq1Mode = createRadio();
	seq1Mode.option("VCO");
	seq1Mode.option("SUB1");
	seq1Mode.option("SUB2");
	seq1Mode.selected('VCO');
	seq2Mode = createRadio();
	seq2Mode.option("VCO");
	seq2Mode.option("SUB1");
	seq2Mode.option("SUB2");
	seq2Mode.selected('VCO');
	
	
	r1s1 = createCheckbox('SEQ 1', true);
	r1s1.changed(checkBoxEvent);
	r1s2 = createCheckbox('SEQ 2', false);
	r1s2.changed(checkBoxEvent);
	r2s1 = createCheckbox('SEQ 1', false);
	r2s1.changed(checkBoxEvent);
	r2s2 = createCheckbox('SEQ 2', false);
	r2s2.changed(checkBoxEvent);
	r3s1 = createCheckbox('SEQ 1', false);
	r3s1.changed(checkBoxEvent);
	r3s2 = createCheckbox('SEQ 2', false);
	r3s2.changed(checkBoxEvent);
	r4s1 = createCheckbox('SEQ 1', false);
	r4s1.changed(checkBoxEvent);
	r4s2 = createCheckbox('SEQ 2', false);
	r4s2.changed(checkBoxEvent);

	//This function will format colors and positions of the DOM elements (sliders, button and text)
	formatDOMElements();
}

function checkBoxEvent() {
	if(r1s1.checked() && r1s2.checked()) {
		rhythms[0] = 3;
	} else if(r1s1.checked()) {
		rhythms[0] = 1;
	} else if(r1s2.checked()) {
		rhythms[0] = 2;
	} else {
		rhythms[0] = 0;
	}
	
	if(r2s1.checked() && r2s2.checked()) {
		rhythms[1] = 3;
	} else if(r2s1.checked()) {
		rhythms[1] = 1;
	} else if(r2s2.checked()) {
		rhythms[1] = 2;
	} else {
		rhythms[1] = 0;
	}
	
	if(r3s1.checked() && r3s2.checked()) {
		rhythms[2] = 3;
	} else if(r3s1.checked()) {
		rhythms[2] = 1;
	} else if(r3s2.checked()) {
		rhythms[2] = 2;
	} else {
		rhythms[2] = 0;
	}
	
	if(r4s1.checked() && r4s2.checked()) {
		rhythms[3] = 3;
	} else if(r4s1.checked()) {
		rhythms[3] = 1;
	} else if(r4s2.checked()) {
		rhythms[3] = 2;
	} else {
		rhythms[3] = 0;
	}
	
}

function draw() {
    //store values in the buffer
	buffer[osc1Offset]=wave_types[waveType.value()];
	buffer[osc1Offset+1]=subOsc1.value();
	buffer[osc1Offset+2]=subOsc2.value();
	
	buffer[osc2Offset]=wave_types[waveType2.value()];
	buffer[osc2Offset+1]=sub2Osc1.value();
	buffer[osc2Offset+2]=sub2Osc2.value();
	
	buffer[subOscOffset]=subOsc1Amp.value();
	buffer[subOscOffset+1]=subOsc2Amp.value();
	buffer[subOscOffset+2]=sub2Osc1Amp.value();
	buffer[subOscOffset+3]=sub2Osc2Amp.value();
	buffer[scaleOffset]=scale_types[scale.value()];
	
	buffer[envelopeParamsOffset]=ampAtk.value();
	buffer[envelopeParamsOffset+1]=ampDecay.value();
	buffer[envelopeParamsOffset+2]=freqAtk.value();
	buffer[envelopeParamsOffset+3]=freqDecay.value();
	buffer[envelopeEgOffset]=freqEg.value();
	
	buffer[seq1BeatOffset]=seq1Step1.value();
	buffer[seq1BeatOffset+1]=seq1Step2.value();
	buffer[seq1BeatOffset+2]=seq1Step3.value();
	buffer[seq1BeatOffset+3]=seq1Step4.value();
	
	buffer[seq2BeatOffset]=seq2Step1.value();
	buffer[seq2BeatOffset+1]=seq2Step2.value();
	buffer[seq2BeatOffset+2]=seq2Step3.value();
	buffer[seq2BeatOffset+3]=seq2Step4.value();
	
	buffer[seqModeOffset]=seq_mode_types[seq1Mode.value()];
	buffer[seqModeOffset+1]=seq_mode_types[seq2Mode.value()];
	buffer[seqRangeOffset]=range_types[seqRange.value()];
	
	buffer[rhythmTargetsOffset]=rhythms[0];
	buffer[rhythmTargetsOffset+1]=rhythms[1];
	buffer[rhythmTargetsOffset+2]=rhythms[2];
	buffer[rhythmTargetsOffset+3]=rhythms[3];
	
	buffer[rhythmDivsOffset]=rhythm1.value();
	buffer[rhythmDivsOffset+1]=rhythm2.value();
	buffer[rhythmDivsOffset+2]=rhythm3.value();
	buffer[rhythmDivsOffset+3]=rhythm4.value();
	
	//send buffer to audio application
	Bela.data.sendBuffer(0, 'float', buffer);

}

function formatDOMElements() {
	let startOsc1W = 100;
	let startOsc1H = 100;
	let s = 10;
	let sh = 50;
	PsubOsc1.position(startOsc1W,startOsc1H);
	subOsc1.position(startOsc1W,startOsc1H);
	PsubOsc2.position(startOsc1W + subOsc1.width+s,startOsc1H);
	subOsc2.position(startOsc1W + subOsc1.width+s,startOsc1H);
	PsubOsc1Amp.position(startOsc1W, startOsc1H + sh);
	subOsc1Amp.position(startOsc1W, startOsc1H + sh);
	PsubOsc2Amp.position(startOsc1W + subOsc1Amp.width+s, startOsc1H + sh);
	subOsc2Amp.position(startOsc1W + subOsc1Amp.width+s, startOsc1H + sh);
	waveType.position(startOsc1W, startOsc1H + 2 * sh);

	let startOsc2W = 550;
	let startOsc2H = 100;
	Psub2Osc1.position(startOsc2W,startOsc2H);
	sub2Osc1.position(startOsc2W,startOsc2H);
	Psub2Osc2.position(startOsc2W + subOsc2.width+s,startOsc2H);
	sub2Osc2.position(startOsc2W + subOsc2.width+s,startOsc2H);
	Psub2Osc1Amp.position(startOsc2W, startOsc1H + sh);
	sub2Osc1Amp.position(startOsc2W, startOsc1H + sh);
	Psub2Osc2Amp.position(startOsc2W + subOsc2Amp.width+s, startOsc2H + sh);
	sub2Osc2Amp.position(startOsc2W + subOsc2Amp.width+s, startOsc2H + sh);
	waveType2.position(startOsc2W, startOsc2H + 2 * sh);
	
	
	let startVCW = 975;
	let startVCH = 100;
	
	PfreqAtk.position(startVCW, startVCH);
	freqAtk.position(startVCW, startVCH);
	PfreqDecay.position(startVCW + freqAtk.width+s, startVCH);
	freqDecay.position(startVCW + freqAtk.width+s, startVCH);
	PampAtk.position(startVCW, startVCH + sh);
	ampAtk.position(startVCW, startVCH + sh);
	PampDecay.position(startVCW + ampAtk.width+s, startVCH + sh);
	ampDecay.position(startVCW + ampAtk.width+s, startVCH + sh);
	freqEg.position(startVCW + (ampAtk.width+s)/2, startVCH + 2 * sh);
	PfreqEg.position(startVCW + (ampAtk.width+s)/2, startVCH + 2 * sh);
	
	let startScaleW = 150;
	let startScaleH = 275;
	
	Pscale.position(startScaleW, startScaleH);
	scale.position(startScaleW, startScaleH);
	PseqRange.position(startScaleW + 350, startScaleH);
	seqRange.position(startScaleW + 350, startScaleH);

	let startSeq1W = 100;
	let startSeq1H = 450;
	
	Pseq1Step1.position(startSeq1W, startSeq1H);
	seq1Step1.position(startSeq1W, startSeq1H);
	Pseq1Step2.position(startSeq1W + seq1Step1.width+s, startSeq1H);
	seq1Step2.position(startSeq1W + seq1Step1.width+s, startSeq1H);
	Pseq1Step3.position(startSeq1W + 2*(seq1Step1.width+s), startSeq1H);
	seq1Step3.position(startSeq1W + 2*(seq1Step1.width+s), startSeq1H);
	Pseq1Step4.position(startSeq1W + 3*(seq1Step1.width+s), startSeq1H);
	seq1Step4.position(startSeq1W + 3*(seq1Step1.width+s), startSeq1H);
	seq1Mode.position(startSeq1W, startSeq1H + sh);
	
	let startSeq2W = 700;
	let startSeq2H = 450;
	
	Pseq2Step1.position(startSeq2W, startSeq2H);
	seq2Step1.position(startSeq2W, startSeq2H);
	Pseq2Step2.position(startSeq2W + seq1Step1.width+s, startSeq2H);
	seq2Step2.position(startSeq2W + seq2Step1.width+s, startSeq2H);
	Pseq2Step3.position(startSeq2W + 2*(seq2Step1.width+s), startSeq2H);
	seq2Step3.position(startSeq2W + 2*(seq2Step1.width+s), startSeq2H);
	Pseq2Step4.position(startSeq2W + 3*(seq2Step1.width+s), startSeq2H);
	seq2Step4.position(startSeq2W + 3*(seq2Step1.width+s), startSeq2H);
	seq2Mode.position(startSeq2W, startSeq2H + sh);
	
	
	
	let startRhythmW = 400;
	let startRhythmH = 600;
	
	Prhythm1.position(startRhythmW, startRhythmH);
	rhythm1.position(startRhythmW, startRhythmH);
	Prhythm2.position(startRhythmW + 1*(rhythm1.width+s), startRhythmH);
	rhythm2.position(startRhythmW + 1*(rhythm1.width+s), startRhythmH);
	Prhythm3.position(startRhythmW + 2*(rhythm1.width+s), startRhythmH);
	rhythm3.position(startRhythmW + 2*(rhythm1.width+s), startRhythmH);
	Prhythm4.position(startRhythmW + 3*(rhythm1.width+s), startRhythmH);
	rhythm4.position(startRhythmW + 3*(rhythm1.width+s), startRhythmH);
	
	r1s1.position(startRhythmW, startRhythmH + sh);
	r2s1.position(startRhythmW + 1*(rhythm1.width+s), startRhythmH + sh);
	r3s1.position(startRhythmW + 2*(rhythm1.width+s), startRhythmH + sh);
	r4s1.position(startRhythmW + 3*(rhythm1.width+s), startRhythmH + sh);

	r1s2.position(startRhythmW, startRhythmH + sh + 30);
	r2s2.position(startRhythmW + 1*(rhythm1.width+s), startRhythmH + sh + 30);
	r3s2.position(startRhythmW + 2*(rhythm1.width+s), startRhythmH + sh + 30);
	r4s2.position(startRhythmW + 3*(rhythm1.width+s), startRhythmH + sh + 30);
}

