# wiiu-tests
A set of applications for the Wii U which are used for testing the behaviour of decaf-emu.

# Building
## Requirements:
- cmake using a Make or Ninja build generator on Windows
- latte-assembler which can be found in decaf-emu, if you want a binary you can download artifacts from the [latest CI run](https://github.com/decaf-emu/decaf-emu/actions)
- [AMD GPU ShaderAnalyzer](https://gpuopen.com/archive/gpu-shaderanalyzer/)
- [devkitPro with wut](https://github.com/devkitPro/wut)

## CMake
Required configuration:
- `-DLATTE_ASSEMBLER=/path/to/latte-assembler.exe`
- `-DAMD_SHADER_ANALYZER=/path/to/GPUShaderAnalyzer.exe`

Recommend configuration:
- `-DCMAKE_INSTALL_PREFIX=/path/to/sdcard/wiiu-tests` - you can then install straight into the layout expected for tests to run
- If you're on Windows you might have trouble using Visual Studio so be sure to pass in a makefile based generator such as -G "Unix Makefiles" or -G "Ninja"

Build:
- `export DEVKITPRO=/path/to/devkitpro`
- `mkdir build`
- `cd build`
- `cmake ../content -DLATTE_ASSEMBLER=/path/to/latte-assembler.exe -DAMD_SHADER_ANALYZER=/path/to/GPUShaderAnalyzer.exe -DCMAKE_INSTALL_PREFIX=/path/to/sdcard/wiiu-tests -G "Ninja"`
- `ninja install`

# Running
It is expected that the output of install is in a folder on the root of your sdcard named wiiu-tests, e.g. such that `sdcard:/wiiu-tests/content/shaders/pos_colour.gsh` exists.

You must configure decaf to point to this sdcard folder, in config.toml this is under the `[system]` group, `sdcard_path=/path/to/sdcard`

These tests can also be run on hardware, just ensure the content is in the wiiu-tests folder as describe above and then go on to use HBL or wiiload to run individual RPX files.
