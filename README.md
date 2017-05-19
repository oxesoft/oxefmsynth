# Oxe FM Synth
Welcome to the [Oxe FM Synth](https://oxesoft.wordpress.com/) official repository.

![defaultskin](https://oxesoft.files.wordpress.com/2007/12/screenshot_full.png)

This repository contains the synth core and three different ways to use it:

- as a [demo](https://en.wikipedia.org/wiki/Demoscene) (for testing synth engine, for profiling or just for fun)
- as a standalone application (for testing GUI)
- as a [VST plugin](https://pt.wikipedia.org/wiki/Virtual_Studio_Technology) for Windows (32/64bit), Linux 32/64bit and Mac OS X (64bit)

## Building

### Windows
Requirements: [mingw-w64](http://mingw-w64.org/) and [msys](https://msysgit.github.io/).
Install both 32 and 64bit (run the installer twice).
On the msys shell just type ``mingw32-make``.

### Linux
To build native executables just type ``make`` (requirement: g++).
To build Windows executables on Ubuntu, type ``sudo apt-get install mingw-w64`` to install the required tools.

### Mac OS X
On Xcode, install the "Command Line Tools" and type ``make`` on terminal. The Xcode IDE itself is not used.

All the code is always compiled by once on purpose, to make sure everything is fine (because it is a tiny project).
``converter`` is a development tool for creating the demos. It converts a .mid file in a tune.h used for the demo player.

## Using the standalone
While we are working on GUI (changing code or making skins) this standalone version helps a lot,
because it is faster than open the plugin host, create a track, add the plugin, and so on.
With the standalone version we reduce it in only one step.
To cross-compile it for Windows, from Linux, run ``make -f Makefile.standalone TARGET=WIN32``.
To build a native version, just type ``make -f Makefile.standalone``.


## Using the demo
By default, a native plataform executable is generated.
The tune can be rendered to a .wav file, to the memory (for profiling purposes) and, optionally, to the sound card in real time, using DirectSound.
You can enable this building it with ``make -f Makefile.demo WITH_DSOUND=YES``. Once it is done, the default behaviour is to start playing. The command
line parameters ``-f`` (to file) and ``-m`` (to memory) still works if used.
To cross-compile it for Windows, from Linux, run ``make -f Makefile.demo TARGET=WIN32``.
To build a native version, just type ``make -f Makefile.demo``.

## Skins
To use a ready skin or test a new one, just put the images in a "skin" dir in the same dir as the plugin/standalone executable.

![skin1](https://oxesoft.files.wordpress.com/2015/04/layzer.png)

![skin2](https://oxesoft.files.wordpress.com/2015/05/snow.png)

![skin3](https://oxesoft.files.wordpress.com/2015/08/totolitoto.png)

![skin4](https://oxesoft.files.wordpress.com/2015/05/tx802.png)

![skin5](https://oxesoft.files.wordpress.com/2015/10/dx7.png)

![skin6](https://oxesoft.files.wordpress.com/2016/05/fm8like.png)
