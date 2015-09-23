Oxe FM Synth
============
Welcome to the [Oxe FM Synth](http://www.oxesoft.com/) official repository.

![screenshot](http://www.oxesoft.com/images/screenshot_medium.png)

This repository contains the synth core and three different ways to use it:

- as a [demo](https://en.wikipedia.org/wiki/Demoscene) (for testing synth engine, for profiling or just for fun)
- as a standalone application (for testing GUI)
- as a [VST plugin](https://pt.wikipedia.org/wiki/Virtual_Studio_Technology) for Windows 32/64 bit and Linux

Building
--------
Requirement: [mingw-w64](http://mingw-w64.org/).
On Ubuntu, type ``sudo apt-get install mingw-w64``.
On Windows, install both 32 and 64bit (run the installer twice).

To build on Windows (on the [msys](https://msysgit.github.io/) shell) just type ``mingw32-make``.
On Linux just type ``make``.
All the code is always compiled by once on purpose, to make sure everything is fine (because it is a tiny project).
``converter`` is a development tool for creating the demos. It converts a .mid file in a tune.h used for the demo player.

Using the demo
--------------
By default, a native plataform executable is generated.
The tune can be rendered to a .wav file, to the memory (for profiling purposes) and, optionally, to the sound card in real time, using DirectSound. You can enable this building it with ``make -f Makefile.demo WITH_DSOUND=YES``. Once it is done, the default behaviour is to start playing. The command line parameters ``-f`` (to file) and ``-m`` (to memory) still works if used.

Skins
-----
To build the synth plugin with your favourite skin, just change the path in the *Makefile.vstplugin*.
To use a ready skin or test a new one, just put the images in a "skin" dir in the same dir as oxevst[64].dll.

![skin1](http://www.oxesoft.com/wordpress/wp-content/uploads/2015/04/layzer-300x206.png) ![skin2](http://www.oxesoft.com/wordpress/wp-content/uploads/2015/05/tx802-300x207.png) 

![skin3](http://www.oxesoft.com/wordpress/wp-content/uploads/2015/05/snow-300x207.png) ![skin4](http://www.oxesoft.com/wordpress/wp-content/uploads/2015/08/totolitoto-300x208.png)
