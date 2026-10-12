/*
    SPDX-FileCopyrightText: 2026 Keypulse contributors
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <input_event_spy.h>
#include <plugin.h>

namespace KWin
{

struct KeyboardKeyEvent;

/**
 * Passive KWin input spy that reports key-down activity without exposing key identity.
 *
 * Only KeyboardKeyState::Pressed is forwarded. Key codes, text, and modifiers are
 * never read into D-Bus messages or logs.
 */
class KeypulsePlugin : public Plugin, public InputEventSpy
{
public:
    KeypulsePlugin();

    void keyboardKey(KeyboardKeyEvent *event) override;
};

} // namespace KWin
