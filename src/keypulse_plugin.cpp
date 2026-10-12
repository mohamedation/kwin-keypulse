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
    // Broadcast to every subscriber. No arguments: key identity must never leave this process.
    const QDBusMessage message = QDBusMessage::createSignal(
        QStringLiteral("/local/keypulse/Emitter"),
        QStringLiteral("local.keypulse.Activity1"),
        QStringLiteral("KeyActivity"));
    QDBusConnection::sessionBus().send(message);
}

KeypulsePlugin::KeypulsePlugin()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService(QStringLiteral("local.keypulse.Emitter"))) {
        qWarning() << "kwin-keypulse: failed to claim local.keypulse.Emitter:"
                    << bus.lastError().message();
    } else {
        qInfo() << "kwin-keypulse loaded (emitting anonymized key-down pulses on"
                << "local.keypulse.Emitter)";
    }

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
