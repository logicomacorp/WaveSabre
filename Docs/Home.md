# WaveSabre Wiki

## Building

- WaveSabre builds have only been tested with Visual Studio 2013/2017 and version 3.6.6 of the VST3 SDK.
- Due to licensing requirements, WaveSabre requires you to [download](https://www.steinberg.net/sdk_downloads/vstsdk366_27_06_2016_build_61.zip) and copy the VST3 SDK into the build yourself. Download, extract, and copy into the *"Vst3.x"* folder. See [this readme](https://github.com/logicomacorp/WaveSabre/blob/master/Vst3.x/README).

### CMake

WaveSabre has a new [CMake](https://cmake.org/) based build-system that can generate Visual Studio project files for you.

- Install CMake v3.11 or later
  - The latest installer [can be found here](https://cmake.org/download/#latestbinary)
  - Alternatively, you can install CMake from [Chocolatey](https://chocolatey.org/) or other package managers.
- To avoid overwriting the in-repo Visual Studio project files, it's *strongly* recommended to build out-of-tree.
- Run `cmake -B build` to generate the Visual Studio project files and solution
- Open the generated solution from the build directory, and proceed as normal.

### PreMake + Visual Studio

WaveSabre can also use Premake 5.0 to generate project files.
- Download a Premake 5.0 binary from here: http://premake.github.io/download.html
- Unzip, and put premake5.exe in a system folder so that it exists in $PATH
- run `premake5 $BUILDTARGET --vstdir=$VSTPATH`
  - `$BUILDTARGET` is the build system you want to generate a build for, e.g. *vs2013*. See premake documentation for other options.
  - `$VSTPATH` is the location of your VST plugins, e.g `"C:\Program Files (x86)\Steinberg\VstPlugins"`. WaveSabre will copy DLLs here after successful builds.
- The VST project files will be generated in a *build* subfolder. Open the solution from the project root, and proceed as normal.
- Note that most of the C++ projects in the repo only have proper Release configurations, and may fail to build in Debug. This is normal; prefer using Release builds.

## New Device Check List

- Add device to WaveSabreCore/Devices.h
- Add device to WaveSabreConvert/Song.cs
- Add device to WaveSabrePlayer/include/SongRenderer.h
- Add device to WaveSabreStandAlongPlayer/main.cpp
- Add device to Vsts/premake5.lua
- Re-run cmake or premake to create new Vst project file
- Add Build dependencies for WaveSabreCore and WaveSabrePlayer to VST Project
- Profit!


## Misc
- Synth seminar from TG: https://www.youtube.com/watch?v=wLX156OVFTA
