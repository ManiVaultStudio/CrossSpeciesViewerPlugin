#pragma once

#include "SharedSettingsAction.h"

// ManiVault includes
#include <ViewPlugin.h>

#include <memory>

using namespace hdps::plugin;
using namespace hdps::gui;

// =============================================================================
// ViewPlugin
// =============================================================================

class CrossSpeciesSharedSettingsPlugin : public ViewPlugin
{
    Q_OBJECT

public:
    CrossSpeciesSharedSettingsPlugin(const hdps::plugin::PluginFactory* factory);
    ~CrossSpeciesSharedSettingsPlugin();

    void init() override;

public: // Serialization

    /**
        * Load plugin from variant map
        * @param Variant map representation of the plugin
        */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
        * Save plugin to variant map
        * @return Variant map representation of the plugin
        */
    QVariantMap toVariantMap() const override;

private:
    std::unique_ptr<SharedSettingsAction>     _settingsAction;
       
};

// =============================================================================
// PluginFactory
// =============================================================================

class CrossSpeciesViewerFactory : public ViewPluginFactory
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID   "nl.BioVault.CrossSpeciesSharedSettingsPlugin"
            FILE  "CrossSpeciesSharedSettingsPlugin.json")
        Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)

public:
    CrossSpeciesViewerFactory() {}
    ~CrossSpeciesViewerFactory() override {}

    /** Returns the plugin icon */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    CrossSpeciesSharedSettingsPlugin* produce() override;

    hdps::DataTypes supportedDataTypes() const override;

    hdps::gui::PluginTriggerActions getPluginTriggerActions(const hdps::Datasets& datasets) const override;
};
