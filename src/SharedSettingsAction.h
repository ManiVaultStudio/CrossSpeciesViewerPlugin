#pragma once

#include "actions/Actions.h"
#include "actions/ColorMap1DAction.h"
#include <PointData/DimensionPickerAction.h>

using namespace hdps;
using namespace hdps::gui;

class SharedSettingsAction : public WidgetAction
{
protected:

    class Widget : public WidgetActionWidget {
    public:
        Widget(QWidget* parent, SharedSettingsAction* sharedSettingsAction);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new SharedSettingsAction::Widget(parent, this);
    };

public:
    SharedSettingsAction(QObject* parent);

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

public: // Action getters
    OptionAction& getClusterColorOptionAction() { return _clusterColorOptionAction; }
    DimensionPickerAction& getSelectedIdAction() { return _selectedGeneNameAction; }
    ColorMap1DAction& getColorMapAction() { return _colorMapAction; }
    DecimalAction& getSizeAction() { return _sizeAction; }
    DecimalAction& getOpacityAction() { return _opacityAction; }

private:
    OptionAction                         _clusterColorOptionAction;
    DimensionPickerAction                _selectedGeneNameAction;
    ColorMap1DAction                     _colorMapAction;
    DecimalAction                        _sizeAction;
    DecimalAction                        _opacityAction;
};