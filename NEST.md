# Nest

This and other 15-466 base code is built around a collection of useful libraries and functions which we will collectively call "nest".
This name captures the goal of having the various parts and functions relatively easy to reconfigure (and remove), while still forming a good support for your game.

## What Is Included

Here is a quick overview of what is included. For further information, ☺read the code☺ !
- Base code (files you will certainly edit):
	- [`main.cpp`](main.cpp) creates the game window and contains the main loop. Set your window title, size, and initial Mode here.
	- [`PlayMode.hpp`](PlayMode.hpp), [`PlayMode.cpp`](PlayMode.cpp) declaration+definition for a basic PPU demonstration. You'll probably build your game on it.
	- [`Jamfile`](Jamfile) responsible for telling FTJam how to build the project. Change this when you add additional .cpp files and to change your runtime executable's name.
	- [`.gitignore`](.gitignore) ignores generated files. You will need to change it if your executable name changes. (If you find yourself changing it to ignore, e.g., your editor's swap files you should probably, instead, be investigating making this change in the global git configuration.)
- Useful code (files you should investigate, but probably won't change):
	- [`Sound.hpp`](Sound.hpp), [`Sound.cpp`](Sound.cpp) `Sound` namespace, functions for `Sample` loading and playback in 2D and 3D.
	- [`Mesh.hpp`](Mesh.hpp), [`Mesh.cpp`](Mesh.cpp) mesh loading.
	- [`Scene.hpp`](Scene.hpp), [`Scene.cpp`](Scene.cpp) scene (transform hierarchy) loading and display (hmm, you might actually edit this code a bit).
	- shaders (you might also build on these:
		- [`ColorProgram.hpp`](ColorProgram.hpp), [`ColorProgram.cpp`](ColorProgram.cpp) GLSL shader that draws objects with vertex colors.
		- [`ColorTextureProgram.hpp`](ColorTextureProgram.hpp), [`ColorTextureProgram.cpp`](ColorTextureProgram.cpp) GLSL shader that draws objects with vertex colors and textures.
		- [`LitColorTextureProgram.hpp`](LitColorTextureProgram.hpp), [`LitColorTextureProgram.cpp`](LitColorTextureProgram.cpp) GLSL shader that draws objects with vertex colors, textures, and lighting.
	- [`DrawLines.hpp`](DrawLines.hpp), [`DrawLines.cpp`](DrawLines.cpp) draw lines in a 3D scene. Very useful for debugging.
	- [`PathFont.hpp`](PathFont.hpp), [`PathFont.cpp`](PathFont.cpp) line-based font, used by DrawLines for text drawing.
	- [`read_write_chunk.hpp`](read_write_chunk.hpp) templated helpers for reading chunk-based binary formats.
	- [`Load.hpp`](Load.hpp), [`Load.cpp`](Load.cpp) asset loading wrapper; load things in the global scope but not until after an OpenGL context is established.
	- [`Mode.hpp`](Mode.hpp), [`Mode.cpp`](Mode.cpp) base class for modes (things that recieve events and draw).
	- [`gl_compile_program.hpp`](gl_compile_program.hpp), [`gl_compile_program.cpp`](gl_compile_program.cpp) helper function to compiles OpenGL shader programs.
	- [`load_save_png.hpp`](load_save_png.hpp), [`load_save_png.cpp`](load_save_png.cpp) helper functions to load and save PNG images.
	- [`GL.hpp`](GL.hpp), [`GL.cpp`](GL.cpp) includes OpenGL 3.3 prototypes without the namespace pollution of (e.g.) SDL's OpenGL header; on Windows, deals with some function pointer wrangling.
	- [`gl_errors.hpp`](gl_errors.hpp) provides a `GL_ERRORS()` macro.
	- [`.github/workflows/build-workflow.yml`](.github/workflows/build-workflow.yml) sets up the repository to be built via github actions whenever it is pushed or released.
	- Asset Viewers:
		- [`show-meshes.cpp`](show-meshes.cpp), [`ShowMeshesMode.hpp`](ShowMeshesMode.hpp), [`ShowMeshesMode.cpp`](ShowMeshesMode.cpp) -- builds `scene/show-meshes` which can view `.pnct` files.
		- [`show-scene.cpp`](show-scene.cpp), [`ShowSceneMode.hpp`](ShowSceneMode.hpp), [`ShowSceneMode.cpp`](ShowSceneMode.cpp) -- builds `scene/show-scene` which can view `.scene` files.
		- shaders used by these helpers:
			- [`ShowMeshesProgram.hpp`](ShowMeshesProgram.hpp), [`ShowMeshesProgram.cpp`](ShowMeshesProgram.cpp)
			- [`ShowSceneProgram.hpp`](ShowSceneProgram.hpp), [`ShowSceneProgram.cpp`](ShowSceneProgram.cpp)
- Here be dragons (files you probably don't need to look at):
	- [`load_wav.hpp`](load_wav.hpp), [`load_wav.cpp`](load_wav.cpp) helper to load wav files. (used by `Sound::Sample`)
	- [`load_opus.hpp`](load_opus.hpp), [`load_opus.cpp`](load_opus.cpp) helper to load opus files. (used by `Sound::Sample`)
	- [`make-GL.py`](make-GL.py) does what it says on the tin. Included in case you are curious. You won't need to run it.
	- [`glcorearb.h`](glcorearb.h) used by `make-GL.py` to produce `GL.*pp`
	- [`make-PathFont-font.py`](make-PathFont-font.py) processes [`PathFont-font.svg`](PathFont-font.svg) to create [`PathFont-font.cpp`](PathFont-font.cpp) (the line-based font used in the DrawLines code).



## Build Instructions

This code is set up to build across Linux, Windows, and MacOS.

Building will be done from the command prompt, using FTJam, the freetype project's fork of Perforce's excellent "Jam" build tool ([Documentation](https://www.perforce.com/documentation/jam-documentation) page at Perforce, for the curious).

To add or remove files from the build, read and edit the [Jamfile](Jamfile).

### Setup

Setup for your development environment should be relatively simple:

 0. (Optional) Make sure your system is set up to make it easy to use your favorite code editor and git from the command prompt. So much nicer than using a GUI.
 1. Install one of our target C++ compilers:
 	- Linux: g++
	- MacOS: clang++ (from XCode)
	- Windows: Visual Studio Community 2019
 2. Install ftjam from your OS's package manager:
    - Linux: e.g. `sudo apt-get install ftjam`
	- MacOS: e.g. `brew install ftjam`
	- Windows: (skip this step; jam for windows is included in the nest-libs package)
 3. Extract an appropriate [release of nest-libs](https://github.com/15-466/nest-libs/releases) to a sibling of this folder:
    - Linux: https://github.com/15-466/nest-libs/releases/download/v0.5/nest-libs-linux-v0.5.tar.gz
	- MacOS: https://github.com/15-466/nest-libs/releases/download/v0.5/nest-libs-macos-v0.5.tar.gz
	- Windows: https://github.com/15-466/nest-libs/releases/download/v0.5/nest-libs-windows-v0.5.zip

Once you are finished, your directory tree should looks something like this:

```
game-programming/ #can be called anything
	nest-libs/    #nest-libs from that repository's releases page
		windows/  #subdirectory name varies depending on platform
		...
	my-gameN/     #fork of the baseN code; can be called anything
		...
	...           #further subdirectories for other games
```

### Building

Once you have your development environment set up, building is as simple as opening a command prompt (see Windows Note below), changing to the game directory, and running `jam`.

Here are a few worthwhile variations:

```
# Note: Always build from the game's root folder:
  $ cd game-programming
  $ cd my-game0

# Simplest build command:
  $ jam

# Variation: multiple (six, in this case) build jobs in parallel:
  $ jam -j6

# Variation: quit if any build job fails:
  $ jam -q

# Variation: run game if build succeeds:
  $ jam && dist/pong

# Variation: show commands being run (useful for debugging the Jamfile):
  $ jam -dx

# Useful: show summary of jam flags:
  $ jam -h

# Variation: this is what I generally use:
  $ jam -q -j6 && dist/pong

# Useful: delete all built files:
  $ jam clean
```


*Windows Note:* a pre-compiled `jam.exe` and a .bat file + .lnk to launch a VS2019 command prompt with jam in the `%PATH%` are included in the `nest-libs/windows/jam/` directory. The README.md in that folder explains how to use them.


## A Word About Github Actions

This repository is equipped with a `.github/workflows/build-workflow.yml` file that tells github that you would like it to build the code for you whenever you push code.
This is a great way to check if things are working cross-platform and even to package releases of your game (the workflow is set up such that if you create a release through the github web UI, it will automatically build, package, and upload binaries to the release).

It can also be a frustrating and time-wasting trap to try to debug any build failures solely using github actions. Use it as a check, but not a development environment.
