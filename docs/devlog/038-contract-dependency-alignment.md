# Contract Dependency Alignment

## Goal

Publish a monophonic package that resolves the same firmware contract version
used by current consumers.

## Problem

Package `0.1.4` still pinned `firmware-contracts` to the older package commit
`c9bde598cff9`. That version predates `ControlChangeEvent`. When a downstream
package also required the newer contract commit, PlatformIO could install both
versions and compile against the older headers, breaking the isolated AMY
consumer.

This was a manifest error, not a reason to add local path fallbacks or restore
a repository-root package.

## Change

Package `0.1.5` pins `firmware-contracts` to commit `c62653e916bb`, which is the
contract version used by the current AMY package. The package test project uses
the same pin, keeping local validation aligned with published consumption.

The package remains published from `packages/monophonic-instrument/`; the
repository root is still only a package host.

## Verification

Run the native package tests and the isolated consumer build before tagging
`monophonic-instrument-v0.1.5`. The GitHub Actions publish workflow will repeat
those checks, pack the subdirectory, and publish the package to the PlatformIO
Registry.

The release tag and registry publication are pending at the time of writing.
