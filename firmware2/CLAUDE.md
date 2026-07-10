# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Firmware v2 for a from-scratch 4-digit 7-segment white-LED digital clock, running on an Arduino Uno (ATmega328P / AVR) with a custom PCB. This `firmware2/` directory is a PlatformIO rewrite of the legacy Arduino `.ino` sketch in the sibling `../firmware/` directory — work here, not there. Hardware peripherals: DS1307 RTC, DHT temp/humidity sensor, LDR (auto-brightness), a buzzer (music), and a resistor-ladder button panel read on a single analog pin.

## Commands

Run from `firmware2/`:

```bash
pio run                    # compile (default env: uno)
pio run -t upload          # compile + flash over USB
pio run -t upload -e uno   # explicit environment
pio device monitor         # serial monitor (Serial debug is commented out by default)
```

There is no test suite — this is bare-metal firmware verified on hardware. `sketch.yaml` (fqbn `arduino:avr:uno`) also allows building with `arduino-cli`, but PlatformIO (`platformio.ini`) is the primary toolchain.

## Architecture

### Cooperative multitasking (ArduinoThread)
There is no RTOS. `loop()` in [src/main.cpp](src/main.cpp) does nothing but `cpu.run()`, where `cpu` is a `ThreadController`. Every active subsystem is a `Thread` with a run `interval`, registered via `cpu.add(...)` in `setup()`. A thread runs when its interval elapses. Blocking code stalls everything — the only intentional blocker is `player.playSync(...)`, which busy-loops the buzzer until the tune ends or a key is pressed.

### Header-only singletons — include order matters
Drivers, the music player, and input are written as `.hpp` files that **define a global instance at the bottom of the header** (e.g. `AnalogPanel panel`, `Buzzer buzzer`, `MusicPlayer player`, `DisplayDriver display`, `RTC_DS1307 rtc`). `main.cpp` is essentially the only translation unit and includes everything once; the `#ifndef WLDC_..._H` guards exist so these definitions aren't duplicated. When adding a driver/singleton, follow this pattern and add its include to `main.cpp` in dependency order. `.cpp` files exist only where a genuinely separate translation unit is needed (`Display.cpp`, `Screen.cpp`, `ScreenController.cpp`, `Buffer.cpp`, `utils.cpp`).

### Screens & navigation ([src/lib/](src/lib/), [src/view/](src/view/))
- `Screen` (abstract) has lifecycle `start()` / `render()` / `stop()` plus input callbacks. Each screen has a numeric `Id` (`WLDC_SCREEN_*`, all defined in [src/config.h](src/config.h)).
- `ScreenController` holds the `screens[]` array, tracks `activeScreen`, and `navigate(ScreenID)` switches screens (calling `stop()`/`start()`). It is itself a `Thread` that calls `activeScreen->render()` each tick, and an `InputListener` that forwards key events to the active screen.
- A screen that needs its own timing (e.g. `ChronometerScreen`, `PomodoroScreen`) multiply-inherits `public Screen, public Thread` and is added to `cpu` separately. Register a new screen in both `screen_controller.add(...)` and (if it's a Thread) `cpu.add(...)` in `setup()`, and give it a `WLDC_SCREEN_*` id.

### Input ([src/lib/Input.hpp](src/lib/Input.hpp), [src/drivers/Panel.hpp](src/drivers/Panel.hpp))
`Input` is an abstract `Thread`; `AnalogPanel` implements it by reading one analog pin (`A3`) and mapping voltage bands (`PANEL_RANGE_KEY_*`) to five logical keys (`InputKey`: HOME, VALUE_UP/DOWN, FUNC_LEFT/RIGHT). It debounces via a cooldown and emits three events to `InputListener`s: `keyDown` (press start, can veto), `keyPress` (fires repeatedly while held, carrying elapsed ms — used to detect long-press against `PANEL_LONG_PRESS`), and `keyUp` (release with total duration).

### Display ([src/drivers/Display.h](src/drivers/Display.h) / .cpp)
Software-multiplexes the 4-digit display via **direct AVR port manipulation** — segments on `PORTD`, digit-select on `PORTB` (configurable via `WLDC_DISPLAY_*` macros). The 7-segment font is a PROGMEM ASCII lookup table (`seven_seg_asciis`). Brightness comes from the LDR driver (`autoBrightness`) or manual inc/dec. Supports `print`/`printEnd` (right-aligned) overloads and `printScroll` for marquee text; `isScrolling()` gates screen rendering.

### Audio & the music bytecode ([src/drivers/Buzzer.hpp](src/drivers/Buzzer.hpp), [src/lib/Music*.hpp](src/lib/))
- `Buzzer` sets up **Timer1 as a 31.25 kHz ISR** doing phase-accumulator square-wave synthesis, mixing up to `BZ_MAX_VOICES` (2) voices for polyphony. `tone(hz, voice)` / `mute()` just set/clear per-voice step sizes.
- `MusicPlayer` (a `Thread`) drives playback; one `MusicVoicePlayer` per voice walks that voice's note stream, handling timing ticks, note gaps, ties/slurs (pitch glide), and nested repeats with multiple endings (`RepeatNotation` stack, `MAX_REPEAT_LEVELS`). `MusicHelper` converts note symbols + octave to frequency and handles BPM/beats/tuning/transpose.
- **Songs are PROGMEM `uint8_t` bytecode arrays** authored with the macros in [src/assets/music/base.h](src/assets/music/base.h): `MUSIC_BPM`/`MUSIC_BEATS`, note macros like `NOTE_C4` (expands to symbol+octave), timing macros like `QUARTER_NOTE`/`EIGHTH_NOTE_DOTTED`, `TIE`/`SLUR`, `REST`, and `REPEAT_START`/`REPEAT_END`/`REPEAT_ENDING`. Each array ends with `MUSIC_END`. A complete song is an array of per-voice pointers terminated by `music_sheet_end`. See [_template.hpp](src/assets/music/_template.hpp) for the scaffold and [debug/slur.hpp](src/assets/music/debug/slur.hpp) for a worked example. To add a song, create its `.hpp` under a themed subfolder and include it in [all.hpp](src/assets/music/all.hpp).

### AVR memory discipline
This is a 2 KB-RAM / 32 KB-flash target. Constant tables and all music/font data live in `PROGMEM` and are read with `pgm_read_*`; user-facing strings use `F(...)`. The music format is byte-oriented (`uint8_t`, not `uint16_t`) specifically to save space. Keep new constant data in PROGMEM and prefer the smallest integer types.

### Currently disabled
The DHT sensor is wired in code but commented out in `setup()` (`// cpu.add(&dht)` / `// dht.begin()`), so `HomeScreen`'s temperature/humidity views won't trigger until it's re-enabled.
