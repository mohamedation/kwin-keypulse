# kwin-keypulse

A small Plasma 6 / KWin plugin that turns real typing into **anonymized key-down pulses** over D-Bus. It is not tied to any one app: keyboard-sound tools, haptics helpers, or accessibility add-ons can all consume the same contract.

## Why this exists

On Wayland, apps cannot safely monitor global key activity without special privileges. Putting a tiny spy **inside KWin** keeps raw key identity in the compositor and only exports “a key went down.”

| This plugin does | This plugin does not |
| --- | --- |
| See `KeyboardKeyState::Pressed` | Send key codes, keysyms, or text |
| Skip pulses while the screen is locked | Open `/dev/input` |
| Emit a zero-argument D-Bus signal | Steal / grab the keyboard |

## D-Bus contract

| Field | Value |
| --- | --- |
| Bus name (owned by plugin) | `local.keypulse.Emitter` |
| Object path | `/local/keypulse/Emitter` |
| Interface | `local.keypulse.Activity1` |
| Signal | `KeyActivity()` — **no arguments** |

Introspection: [`dbus/local.keypulse.Activity1.xml`](dbus/local.keypulse.Activity1.xml).

The plugin emits a session-bus signal. Any number of apps can subscribe; if nobody is listening, nothing happens.

## Build and install (user-local, no root)

Requires Plasma 6 / KWin development headers (`kwin`), Qt6 (Core, DBus, Gui, Widgets), and KF6 Config / CoreAddons / WindowSystem. Optional: `extra-cmake-modules` for the official `find_package(KWin)` path.

```bash
cmake -S . -B build
cmake --build build
cmake --install build
```

Default prefix is `~/.local`. The plugin installs to:

```text
~/.local/lib/qt6/plugins/kwin/plugins/libkeypulse.so
```

Then log out/in (or restart the Plasma session) so KWin reloads plugins.

```bash
ls -l ~/.local/lib/qt6/plugins/kwin/plugins/libkeypulse.so
journalctl --user -b | rg -i 'kwin-keypulse' | tail
```

## Test without a full consumer

Watch the signal while typing:

```bash
dbus-monitor --session "type='signal',sender='local.keypulse.Emitter',interface='local.keypulse.Activity1',member='KeyActivity'"
```

Confirm the plugin owns the name:

```bash
busctl --user status local.keypulse.Emitter
```

## Using it from another project

1. Install this plugin for the user’s Plasma session.
2. Subscribe to `local.keypulse.Activity1.KeyActivity` from sender `local.keypulse.Emitter` (path `/local/keypulse/Emitter`).
3. On each signal, play a sound / haptic / etc. Do not expect key identity — there is none.
4. Multiple apps may subscribe at once; none need to own a bus name.


## Privacy

- Only press events are observed; releases and autorepeat are ignored.
- Key identity fields on `KeyboardKeyEvent` are never copied into D-Bus messages or logs.
- Pulses are suppressed while KWin reports the lock screen active.
- Treat this plugin as trusted compositor code (same class as other KWin plugins).

## ABI note

Binary KWin plugins must be **rebuilt for each KWin / Plasma major (often minor) ABI**. That is a KWin limitation, not specific to this project.

## Uninstall

```bash
rm -f ~/.local/lib/qt6/plugins/kwin/plugins/libkeypulse.so
# then restart Plasma session
```

## License

See [`LICENSE`](LICENSE) and SPDX headers in source files.
