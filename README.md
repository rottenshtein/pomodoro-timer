# pomodoro-timer
Simple pomodoro timer.

---
# Requirements
- libnotify
- mINI

# Building
```bash
make
```

# Usage
```bash
pomodoro_timer --help
```

---
# Additional notes

`pomodoro_config.ini` contains example configuration.

You can store config in multiple locations:
```
/etc/pomodoro/
~/.config/pomodoro/
$PWD
```

Or just pass it with `--config <path>`.
