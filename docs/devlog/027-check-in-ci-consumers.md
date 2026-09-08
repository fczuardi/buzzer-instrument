# Slice 027: Check In CI Consumers

The CI workflow used to generate small PlatformIO consumer projects inline with
`cat <<EOF` blocks. That kept early packaging experiments cheap, but the package
layout is stable enough now that the workflow should not carry C++ source and
project files inside YAML.

This slice moves those smoke consumers into checked-in fixture projects:

- `ci/consumers/monophonic-package`
- `ci/consumers/m5-tone-output`

The workflow now remains focused on orchestration: install PlatformIO, run
tests, pack packages, and build the fixture consumers.

Validation targets:

```bash
pio test -d packages/monophonic-instrument -e native
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
pio pkg pack packages/m5-tone-output --output /home/fcz/dev/m5stick/.tmp
pio run -d ci/consumers/monophonic-package
pio run -d ci/consumers/m5-tone-output
```
