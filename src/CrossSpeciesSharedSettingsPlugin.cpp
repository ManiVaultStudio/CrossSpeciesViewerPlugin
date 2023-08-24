#include "CrossSpeciesSharedSettingsPlugin.h"

#include "SharedSettingsAction.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.CrossSpeciesSharedSettingsPlugin")

using namespace hdps;
using namespace hdps::util;

// =============================================================================
// Plugin
// =============================================================================

CrossSpeciesSharedSettingsPlugin::CrossSpeciesSharedSettingsPlugin(const hdps::plugin::PluginFactory* factory)
    : ViewPlugin(factory)
    , _settingsAction(nullptr)
{
}

void CrossSpeciesSharedSettingsPlugin::init()
{
    getWidget().setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    _settingsAction = std::make_unique<SharedSettingsAction>(this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_settingsAction->createWidget(&getWidget()));
    getWidget().setLayout(layout);

    setSerializationName("CrossSpeciesSharedSettings");   
}

void CrossSpeciesSharedSettingsPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Settings");

    _settingsAction->fromVariantMap(variantMap["Settings"].toMap());

}

QVariantMap CrossSpeciesSharedSettingsPlugin::toVariantMap() const
{
    QVariantMap variantMap = ViewPlugin::toVariantMap();

    _settingsAction->insertIntoVariantMap(variantMap);

    return variantMap;
}


// =============================================================================
// Plugin factory
// =============================================================================

QIcon CrossSpeciesViewerFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("table", color);
}

CrossSpeciesSharedSettingsPlugin* CrossSpeciesViewerFactory::produce()
{
    return new CrossSpeciesSharedSettingsPlugin(this);
}

hdps::DataTypes CrossSpeciesViewerFactory::supportedDataTypes() const
{
    return {};
}

hdps::gui::PluginTriggerActions CrossSpeciesViewerFactory::getPluginTriggerActions(const hdps::Datasets& datasets) const
{

    PluginTriggerActions pluginTriggerActions;

    return pluginTriggerActions;
}

