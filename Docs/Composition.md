## Composition - Ableton

Almost everything you do within the DAW is supported apart from the following features.

- Automation of any non-WaveSabre parameters
- MIDI parts which start at less than 0 position will lose notes (we're working on this)
- Only linear automation is supported (don't use curves etc.)

The project file is then parsed by the converter to create the music data required for WaveSabre playback.

You can also use any other VST plugin within your project such as analysers and wave displays but anything not recognised as a WaveSabre plugin will be ignored by the converter.

### Track grouping

There are two methods of grouping tracks. You can use Group Tracks to house multiple tracks and place devices to effect the overall output of those tracks. You can also use an Audio Track. Simply add the track and in the IO section set the monitor to be "IN". For each track you want to group together, change the output of that track from Master to your newly created audio track.

## Composition - Renoise

- Instruments can only route audio to a single track
- Instruments Audio Routing options are supported
  - If set to "Current Track" then the converter will determine the first track it is used on.  Any notes on subsequent tracks will be ignored.
  - If set to specific track, the notes can appear on any track
- Instrument automations can be on any track
- All send devices need to be at the end of the DSP chain
- You can have ONE muted send device and it has to be the last send device in DSP chain
- Group tracks are supported including nesting of group tracks
- No effects commands can be used apart from note velocity, only automations of WaveSabre parameters are allowed
- Global Groove parameters are supported
- Song option of Track Headroom is supported, we think.

### Side chaining

Side chaining is supported through the use of a third party plugin called MetaPlugin which is available from DDMF -> https://ddmf.eu/metaplugin-chainer-vst-au-rtas-aax-wrapper/

Setup on ducking channel

1. Add a MetaPlugin to the track which requires ducking.
2. Within MetaPlugin, add a Smasher and a SendIt
3. Connect the "Audio Input" device to input channels 1 and 2 of Smasher
4. Connect the audio output of SendIt to input channels 3 and 4 of Smasher
5. Connect output channels of Smasher to the "Audio Output" device
6. Within SendIt, set it to Receive and the required Channel

Setup on key channel

1. Add a SendIt device, this has to be at the END of the DSP chain
2. Set it to Send and the same channel as above

## Composition - FL Studio

Most of the basic functions are supported in FL Studio. Obviously more DAW specific items, such as LFO automations and so on are not supported

- Play Truncated Notes On or Off is supported
- Automation and midi clips can have varying start end end points
- Tension value in automation is NOT supported

## Composition - Reaper

Most basic functions should be fine.  It is a very new converter and hasn't been fully tested by the community as yet.