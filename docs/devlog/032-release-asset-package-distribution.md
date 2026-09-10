# Slice 032: Release Asset Package Distribution

## Goal

Provide a reproducible remote distribution path for the
`monophonic-instrument` PlatformIO package without turning the repository root
into a package.

## Problem

This repository is a package host/monorepo. The concrete PlatformIO package
lives at:

```text
packages/monophonic-instrument
```

The repository root must remain non-package because it also contains apps,
devlogs, CI fixtures, and other package directories. A root `library.json`
would make PlatformIO treat unrelated repository content as one library, which
is the wrong boundary.

Local sibling builds can use a `file://` dependency to the concrete package
directory, but remote consumers such as GitHub Actions cannot access a
developer-machine path like `/home/fcz/dev/...`.

## Distribution Decision

Use custom GitHub Release assets.

Important distinction:

- GitHub Packages is not a generic PlatformIO/C++ registry for this use case.
- GitHub's automatic `Source code (tar.gz)` release archive contains the whole
  monorepo and is not the intended PlatformIO package.
- `pio pkg pack packages/monophonic-instrument` creates the correct artifact:
  a tarball whose root content is the concrete package.

PlatformIO can install a remote `.tar.gz` directly, so a release asset URL can
serve as the package dependency for downstream projects.

## Workflow

Added `.github/workflows/release-monophonic-instrument.yml`, triggered by tags
matching:

```text
monophonic-instrument-v*
```

The workflow:

1. checks out the tagged source;
2. installs `platformio==6.1.19`, matching the existing CI;
3. runs native tests for `packages/monophonic-instrument`;
4. builds the isolated package consumer;
5. verifies that the tag version matches
   `packages/monophonic-instrument/library.json`;
6. runs:

   ```sh
   pio pkg pack packages/monophonic-instrument --output "$RUNNER_TEMP/monophonic-package"
   ```

7. creates or updates the GitHub Release for the tag;
8. uploads the packed tarball as the release asset.

The expected asset name for version `0.1.2` is:

```text
monophonic-instrument-0.1.2.tar.gz
```

The corresponding tag is:

```text
monophonic-instrument-v0.1.2
```

## Verification

The release workflow itself runs only after the tag is pushed. Local validation
for this slice should cover the same package checks:

```sh
pio test -d packages/monophonic-instrument -e native
pio pkg pack packages/monophonic-instrument --output /tmp
pio run -d ci/consumers/monophonic-package
pio run -d apps/plus2-buzzer-local-test
pio run -d apps/core-gray-speaker-local-test
```

## Limits

This slice adds the release mechanism but does not by itself prove the final
remote asset URL. That requires pushing a non-conflicting tag and letting the
GitHub Actions release workflow publish the custom package tarball.
