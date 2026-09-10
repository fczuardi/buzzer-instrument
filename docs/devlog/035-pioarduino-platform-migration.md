# pioarduino platform migration

All ESP32 environments in this repository now use the pioarduino
`platform-espressif32` release. The native test environment remains on
`platformio/native`.

This removes the remaining mix with `platformio/espressif32@7.0.1`. The mixed
platforms used incompatible `tool-esptoolpy` versions in the shared PlatformIO
cache, which could leave builds or uploads with incomplete tool installations.
The migration changes the package, local application, and isolated consumer
builds together so they exercise the same ESP32 platform family.

The Core Gray upload speed remains `460800`; Plus2 remains `1500000`. Builds
and physical validation are separate checks for this slice.
