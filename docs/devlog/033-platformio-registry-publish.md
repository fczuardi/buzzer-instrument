# Slice 033: PlatformIO Registry Publishing

## Goal

Automate public PlatformIO Registry publishing for the package directories that
are consumed by sibling repositories and umbrella showcases.

## Decision

`0.x.y` package versions remain intentionally experimental and may break APIs.
The benefit of publishing them anyway is that consumers can use normal
PlatformIO dependency resolution instead of local tarballs, absolute paths, or
checkout-specific `file://` dependencies.

The first manually published packages were:

```text
fcz2/monophonic-instrument@0.1.2
fcz2/m5-tone-output@0.1.2
```

`m5-tone-output@0.1.2` matched the local tarball but still relied on consumers
to provide `monophonic-instrument` explicitly. The next package version is
prepared as `0.1.3` so its manifest declares the real dependency on
`fcz2/monophonic-instrument@0.1.2`.

## Workflow

Added `.github/workflows/publish-platformio.yml`.

It runs on tags shaped like:

```text
monophonic-instrument-v*
m5-tone-output-v*
```

For each tag it:

1. resolves the package directory from the tag prefix;
2. verifies the tag version matches `packages/<package>/library.json`;
3. installs the pinned PlatformIO version;
4. runs package tests when the package has native tests;
5. builds the isolated consumer fixture;
6. runs `pio pkg pack`;
7. publishes with `pio pkg publish` using the `PLATFORMIO_AUTH_TOKEN` secret.

## Verification

Before adding the workflow, both packages were published manually with the local
token and checked through the Registry:

```sh
PLATFORMIO_CORE_DIR=.platformio-home pio pkg show fcz2/monophonic-instrument
PLATFORMIO_CORE_DIR=.platformio-home pio pkg show fcz2/m5-tone-output
```

Local package checks for `m5-tone-output`:

```sh
pio pkg pack packages/m5-tone-output --output /tmp
pio run -d ci/consumers/m5-tone-output
pio run -d apps/plus2-buzzer-local-test
pio run -d apps/core-gray-speaker-local-test
```

All passed before submitting `fcz2/m5-tone-output@0.1.3` to the Registry. At
the time this slice was written, `pio pkg show fcz2/m5-tone-output` still listed
`0.1.2` as the latest visible version, so downstream projects should continue
pinning `0.1.2` until `0.1.3` is confirmed visible.

## Limits

The workflow should publish only future versions. Registry versions are
effectively immutable, so already-published versions such as `0.1.2` should not
be tagged again for publish.
