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

- Each track can only use one instrument
- Do not use the same instrument on two different tracks
- Instrument automations must be on the same track as the instrument
- All send devices need to be at the end of the DSP chain
- You can have ONE muted send device and it has to be the last send device in DSP chain
- Group tracks are supported including nesting of group tracks
- No effects commands can be used, only automations of wavesabre parameters are allowed
- Global Groove parameters are supported

### Side chaining

Sadly this is not possible in Renoise due to its lack of support for multiple channel plugins.

## Composition - FL Studio

Most of the basic functions are supported in FL Studio. Obviously more DAW specific items, such as LFO automations, layering etc. are not supported

- Play Truncated Notes On or Off is supported
- Automation and midi clips can have varying start end end points
- Tension value in automation is NOT supported
