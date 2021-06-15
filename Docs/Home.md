# WaveSabre Wiki

## Building

- WaveSabre builds have only been tested with Visual Studio 2013/2015/2017/2019 and version 3.6.6 of the VST3 SDK.
- Due to licensing requirements, WaveSabre requires you to [download](https://www.steinberg.net/sdk_downloads/vstsdk366_27_06_2016_build_61.zip) and copy the VST3 SDK into the build yourself. Download, extract, and copy into the *"Vst3.x"* folder. See [this readme](https://github.com/logicomacorp/WaveSabre/blob/master/Vst3.x/README).

### CMake

WaveSabre has a new [CMake](https://cmake.org/) based build-system that can generate Visual Studio project files for you.

- Install CMake v3.11 or later
  - The latest installer [can be found here](https://cmake.org/download/#latestbinary)
  - Alternatively, you can install CMake from [Chocolatey](https://chocolatey.org/) or other package managers.
- Run `cmake -B build` to generate the Visual Studio project files and solution
  - Optionally, you can also specify `-DVSTDIR=<some-path>` to copy the VST plugins into your DAW's VST plugin directory upon build.
  - You can also specify `-DVSTSDK3_DIR=<dir>` to use a VSTSDK from outside of the source-tree.
  - As of Visual Studio 2019, `-A Win32` is required to generate a project with 32-bit targets.
- Open the generated solution from the build directory, and proceed as normal.

## New Device Check List

- Add device to WaveSabreCore/Devices.h
- Add device to WaveSabreConvert/Song.cs
- Add device to WaveSabrePlayer/include/SongRenderer.h
- Add device to WaveSabreStandAlongPlayer/main.cpp
- Re-run cmake to create new Vst project file
- Add Build dependencies for WaveSabreCore and WaveSabrePlayer to VST Project
- Profit!
