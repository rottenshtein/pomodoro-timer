# pomodoro-timer
Simple pomodoro timer.

---

# Features
- Pause/resume via linux signals
- Notifications via libnotify
- Configuration file

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

To pause/resume timer you can send SIGUSR1 signal.
