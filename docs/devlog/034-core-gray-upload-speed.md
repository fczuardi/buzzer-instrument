# Core Gray upload speed

The Core Gray local application now uses `upload_speed = 460800`. The faster
`1500000` setting allowed the stub flasher to start but repeatedly lost the
chip during flash verification. This matches the reliable setting used by the
Core Gray showcase and AMY applications.

The Plus2 application remains at `1500000`; its upload path has not shown this
failure. This slice changes configuration only and does not add new hardware
validation.
