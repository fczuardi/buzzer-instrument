# Slice 001: PlatformIO Buzzer Smoke Test

This first slice creates the smallest useful firmware for the buzzer instrument
experiment. The goal is not a musical instrument yet. The goal is to prove that
PlatformIO can build for the M5StickC Plus2 and that the built-in buzzer can
produce a controlled tone.

The firmware initializes M5Unified, configures the display, starts a direct
LEDC/PWM buzzer backend, and plays a short A4 tone at 440 Hz during boot.
Button A repeats the same smoke-test tone. Button B stops the tone if it is
still playing.

The project includes a local PlatformIO board definition for the M5StickC Plus2,
matching the receiver experiment. The pinned `espressif32` platform does not
provide this board ID directly.

The first hardware test used `M5.Speaker.tone()`. It produced sound, but it
sounded more like noise than a clear pitch on this device. The working version
uses ESP32 LEDC directly on GPIO 2 with a 50% duty cycle. That path produced a
recognizable A4 according to Android guitar tuner apps.

The code intentionally uses direct constants instead of a note-to-frequency
module. That keeps this slice focused on hardware output. A later slice can add
pure C++ conversion from MIDI note numbers to frequencies and cover it with
native tests.

Serial output records boot, tone start, tone stop, and uptime messages. This is
useful because the display only shows the current state, while the monitor keeps
the event history.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Hardware test:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run --target upload
env PLATFORMIO_CORE_DIR=.platformio-home pio device monitor
```

Expected result: the device shows the buzzer smoke-test screen, plays one short
A4 tone after boot, plays it again when Button A is pressed, and stops when
Button B is pressed during playback.
