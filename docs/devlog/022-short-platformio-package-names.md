# Slice 022: Short PlatformIO Package Names

The first extracted package was originally named `embedded-music-monophonic`,
with future output packages planned as `embedded-music-m5-buzzer-output` and
`embedded-music-m5-speaker-output`. That repeated the provisional umbrella
project name in every package before the project identity was settled.

This slice changes the package naming plan while the migration is still early:

- `monophonic-instrument` is the shared single-voice instrument policy package.
- `m5-buzzer-output` is the planned M5StickC Plus2 buzzer output package.
- `m5-speaker-output` is the planned M5Unified speaker output package.

The package names intentionally avoid an `embedded-music` or `em` prefix. If the
libraries are later published to the PlatformIO Registry, PlatformIO's
`owner/name@version` dependency form can provide namespacing without baking a
provisional project name into every package.

This slice renames `packages/embedded-music-monophonic` to
`packages/monophonic-instrument`, updates its `library.json` package name, and
updates CI and documentation references. It does not move any hardware output
backend yet.

Validation targets:

```bash
pio test -d packages/monophonic-instrument -e native
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
```
