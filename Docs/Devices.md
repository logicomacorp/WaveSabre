# Current devices

### Slaughter (synth)

- Polyphonic 3 oscillator subtractive synthesiser
- ADSR envelope
- Pitch envelope
- Filter envelope
- Noise

Slaughter is a very useful synth but it is also quite a CPU hungry beast. To make sure your instruments don't kill your CPU, take care with your envelope release times, overlapping notes and level of Unison. If any of the three oscillators are set to a level of zero, they are switched off which improves performance. 

### Falcon (synth)

- 2 oscillator FM synthesiser
- 2 Amp ADSRs, one for master and one modulator
- Feedback and feedforward
- Pitch ADSR Pitch which can modulate master and / or modulator

The naughty step-child of the generators, Falcon can make some rather nasty noises. As with FM synthesis, one oscillator modifies the other and can fade between sine and the first 3 partials of a squarewave (same as square 1 3 5 from FM8). Generally used for horrifically nasty bass death, but can also be used for nice FM plucks or soft sines.

### Specimen (sampler)

- GSM sample player
- Imports 44khz 16-bit mono WAV
- Playback samples on any note with velocity
- Looping with straight or ping-pong
- Sample start position
- Reverse playback
- ADSR
- Filter

Specimen is a sample player. It can import a straight 44khz 16-bit WAV sample and compresses it using GSM. Playback is based on the compressed sample so you know what it will sound like when converted. To import samples at a lower rate, modify the sample using any common editor. The specification of the file still needs to be 44khz 16-bit, so it is advices to use a pitch shift to preserve these attributes rather than actual re-sampling which can change these.

### Adultery (sampler)

- GM.DLS sample player
- Playback samples on any note with velocity
- Looping with straight or ping-pong
- Sample start position
- Reverse playback
- ADSR
- Filter

Adultery uses the same sample engine as Specimen with one small difference. The loop boundary mode will allow to the loop points in the original sample to be used, meaning pitched instruments and looped samples can be played correctly. The additional loop boundary mode sets the entire sample as the loop size.

### Chamber (effect)

- Reverb effect
- High / low cut for effect
- Dry / wet mix
- Pre-Delay

A simple reverb effect, the type is essentially a multiplier for the delay lengths. Also has a Pre-Delay to delay the input source of the reverb.

### Crusher (effect)

- Bit crusher
- Down sampler
- Dry / wet mix

A down sampler and bit crusher. Useful for adding a gritty texture to the source signal.

### Echo (effect)

- Stereo delay effect
- Feedback
- Low and high cut filters
- Dry / wet

Stereo delay plugin which operates two delay lines, one for each channel, which can be individually sync'd to a tempo. Includes low and high cut filters to help tone the delay output

### Leveller (effect)

- 3 band EQ
- High and low cut filters with resonance
- Master level

The leveller is a useful utility plugin which can be used at any part of the signal chain to modify your sound. The high and low cut filters allow for smooth filter sweeps. The master level allows for volume automation on tracks or just a general adjustment.

### Scissor (effect)

- 3 types of distortion (clipper, sine and parabola)

Scissor is a very nasty distortion unit with three types of mangler. It is also useful as a simple signal boost.

### Smasher (effect)

- Compressor with sidechain

Smasher is an compressor with threshold and ratio but can also act as a side-chain compressor. To use it as a sidechain, first add an instance to the channel you want to effect and switch it to sidechain mode. You can do this on an individual channel or a group (see above). To provide the key a channels output needs to be sent to the destination channel, when you select the channel you will see (Smasher 3/4). If the key needs to be heard, i.e. it is provided by your kick drum, then add a send channel and provide it with the kick signal.  On the send channel you route the output to you Smasher 3/4 channel.

### Twister (effect)

- Flange +/-
- Phaser +/-
- Feedback
- Stereo param mode
- Vibrato modulator

Twister is a fairly straight flanger / phaser plugin. There are 4 modes, Flange +, Flange -, Phase + and Phase -. It's unique feature is it's stereo param spread which makes for some really tasty spacial displacement.

### Cathedral (effect)

- Reverb effect
- Freeze mode
- Room size ( with infinity! )
- Width ( stereo )
- Low and high cut filters
- Dry / wet mix
- Pre-Delay

Cathedral is a reverb based on Schroeder/Moorer model. The low and high cut filters allow for tuning the resulting reverb output along with a dry / wet mix. The freeze mode, when switched on, holds the current reverb buffer and prevents any further input. This can be useful for really long tails but also as a form of input gating. Also contains a pre-delay to delay the input source of the reverb.

# Deprecated devices

### Thunder (sampler) (superceded by Specimen)

- GSM sample player
- Imports 44khz 16-bit mono WAV
- Pitch locked to 44khz for all notes
- Only supports note on event
