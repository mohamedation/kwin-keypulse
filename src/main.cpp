/*
    SPDX-FileCopyrightText: 2026 Keypulse contributors
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "keypulse_plugin.h"

#include <plugin.h>

namespace KWin
{

class KeypulsePluginFactory : public PluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginFactory_iid FILE "metadata.json")
    Q_INTERFACES(KWin::PluginFactory)

public:
    explicit KeypulsePluginFactory() = default;

    std::unique_ptr<Plugin> create() const override
    {
        return std::make_unique<KeypulsePlugin>();
    }
};

} // namespace KWin

#include "main.moc"
