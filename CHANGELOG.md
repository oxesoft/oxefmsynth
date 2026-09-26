# Changelog

All notable changes to **Oxe FM Synth** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.0.0-beta.1] - 2026-09-25

### Added
- **CLAP Support:** Added complete implementation of the open [CLAP](https://cleveraudio.org/) (CLever Audio Plug-in) standard (v1.2.10) for macOS, Linux, and Windows with audio processing, parameter automation, MIDI events, state serialization, and GUI hosting.
- **VST3 Support:** Added native VST3 implementation (`COxeVst3` and `COxeVst3Editor`) with automatic SDK download (`make vst3`).
- **Vector GUI Engine (Blend2D):** Modernized entire interface with high-resolution vector graphics using Blend2D, eliminating dependency on raster bitmap files.
- **Real-Time Envelope Visualization:** Added dynamic envelope curve visualizations for operators with active segment and stage indicators.
- **GUI Scaling:** Multi-step zoom scaling support (100%, 125%, 150%, 175%, 200%, 250% via keyboard shortcuts `Cmd/Ctrl+1..6`).

### Removed
- Removed legacy VST 2.4 plugin code and build scripts.
- Removed legacy BMP skins and the `embedresources` bitmap tool.
- Removed deprecated OpenGL dependency across macOS toolkits.

---

## [1.3.5] - 2016-04-19

### Fixed
- Linux: Fixed blank GUI display bug.
- Build: Added support for compiling with clang (`CC=clang CXX=clang++`).

---

## [1.3.4] - 2015-11-17

### Added
- OS X: Accelerated graphics using OpenGL.

### Fixed
- Linux: Fixed potential issue during GUI loading.
- All platforms: Fixed possible crash when the GUI is displayed during DAW project load.

---

## [1.3.3] - 2015-10-20

### Added
- New skin: "DX7" by Layzer.

### Fixed
- Windows: Fixed crash/bug when closing the GUI.
- Linux & OS X: Fixed issue loading external skins.
- OS X: Fixed plugin detection issue where certain hosts failed to recognize the plugin.
- All platforms: Fixed loading error with skins containing old DIB bitmap headers.

---

## [1.3.2] - 2015-10-12

### Added
- OS X: First native OS X release.

### Fixed
- Linux: Fixed crash on closing the GUI window.
- Windows: Version number maintenance bump.

---

## [1.3.1] - 2015-09-26

### Added
- Linux: First native Linux release.

### Fixed
- General stability and crash fixes across plugin hosting.

---

## [1.3.0] - 2015-09-01

### Added
- Full parameter automation support on MIDI channel 1.
- Value tooltips displayed on mouse hover.
- Double-click knob/control to reset to default value.
- Host undo / redo integration support.

### Changed
- All parameter modifications are now applied immediately in real time.
- The "Store" button now functions specifically as a "Copy" utility.

### Fixed
- Fixed fine tune display readout when keyboard tracking is disabled.
- Fixed timing bug in LFO rate control.

### Removed
- Removed the right-click "Compare" function.

---

## [1.2.2] - 2015-08-06

### Added
- 64-bit native builds available for Windows.

---

## [1.2.1] - 2015-05-04

### Added
- Mouse wheel scroll support for knobs and controls.
- Parameter compare function (via right mouse click).
- New skins: "snow" and "tx802", plus 33 new bass presets designed by Layzer.

### Fixed
- Fixed mouse dragging terminating when moving cursor past window boundaries.
- Fixed keyboard focus issue blocking host virtual keyboards in certain DAWs.
- Fixed effects processing glitch.
- Fixed default skin button layout (swapped Store and Program buttons).

---

## [1.2.0] - 2015-04-15

### Added
- Embedded factory soundbanks: Bank 0 by Nolwenn (Annabelle), Bank 1 by Summa & Teksonic.
- MIDI bank select using LSB as bank index.
- Support for loading external user skins.

### Fixed
- Fixed occasional audio clicks/pops on envelope decay stages.
- Fixed sustain pedal (MIDI CC 64) not functioning.
- Fixed synth engine freeze/hang under extended usage.
- Fixed pitch calibration issues at sample rates other than 44.1 kHz.
- Fixed incorrect state persistence on session restore.
- Fixed patch name editing behavior in host environments.
- Improved knob drag physics and response.

### Changed
- Completely standalone operation: zero registry usage and no installer required.

---

## [1.1.2] - 2004-04-01

### Added
- New bandlimited sawtooth and square waveform algorithms.
- Allowed operator frequency to start at 0 Hz for offset modulation.
- Per-program "High Quality" oversampling switch.
- Inverted modulation wheel destinations.

### Fixed
- Fixed audio anomaly occurring when filter resonance was set to zero.
- Fixed release envelope stage on operator Z.
- Corrected units on LCD coarse tune display.
- Fixed channel state persistence on GUI reload.

---

## [1.1.1] - 2004-02-16

### Added
- Voice reallocation algorithm when maximum polyphony is exceeded.
- Workaround for patch renaming inside Tracktion DAW.

### Fixed
- Fixed silence issue in certain VST hosts.
- Improved VST SDK 2.3 compliance (FXP/FXB soundbanks and MIDI program names).
- Fixed LCD display update when patches change via MIDI Program Change.

---

## [1.1.0] - 2004-02-06

### Added
- Soundbank patch navigator.
- Reverb and Delay send knobs per channel.
- Audio output configuration dialog in standalone executable.

### Fixed
- Translated remaining untranslated Portuguese interface text into English.
- Fixed issue where parameter knobs failed to render.
- Fixed transmission of patch names to host DAW.
- Resolved soundbank discrepancy between standalone and VST versions.
- Fixed stuck note issues on rapid MIDI playback.
- Channel state is now cleanly preserved on application exit.

---

## [1.0.0] - 2004-01-02

- Initial public release of Oxe FM Synth.
- 8-operator FM synthesis core with multitimbral 16-channel operation.
