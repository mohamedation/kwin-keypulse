/*
    SPDX-FileCopyrightText: 2026 Keypulse contributors
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keypulse_plugin.h"

#include <core/inputdevice.h>
#include <input.h>
#include <input_event.h>
#include <wayland_server.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

namespace KWin
{

static void notifyListeners()
{
    // Fire-and-forget method call. Any app that owns local.keypulse.Player can react.
    // No arguments: key identity must never leave this process.
    const QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("local.keypulse.Player"),
        QStringLiteral("/local/keypulse/Player"),
        QStringLiteral("local.keypulse.Activity1"),
        QStringLiteral("NotifyKeyActivity"));
    QDBusConnection::sessionBus().send(message);
}

KeypulsePlugin::KeypulsePlugin()
{
    qInfo() << "kwin-keypulse loaded (anonymized key-down pulses only)";
    input()->installInputEventSpy(this);
}

void KeypulsePlugin::keyboardKey(KeyboardKeyEvent *event)
{
    if (!event) {
        return;
    }

    // Pressed only. Ignore releases and autorepeat. Do not inspect key identity fields.
    if (event->state != KeyboardKeyState::Pressed) {
        return;
    }

    if (WaylandServer *ws = waylandServer(); ws && ws->isScreenLocked()) {
        return;
    }

    notifyListeners();
}

} // namespace KWin
