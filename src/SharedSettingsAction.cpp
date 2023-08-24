#include "SharedSettingsAction.h"

using namespace hdps;
using namespace hdps::gui;

SharedSettingsAction::SharedSettingsAction(QObject* parent) :
    WidgetAction(parent, "Shared Settings"),
    _clusterColorOptionAction(this, "Map Coloring", { "Class", "Subclass", "Cross-species Cluster", "Mean Expressions" }),
    _selectedIdAction(this, "Selected Id"),
    _colorMapAction(this, "Color Map"),
    _sizeAction(this, "Point Size", 0.0, 100.0, 10, 10),
    _opacityAction(this, "Opacity", 0.0, 100.0, 50.0, 50.0)
{
    setText("Settings");
    setSerializationName("Settings");

    _sizeAction.setSuffix("px");
    _opacityAction.setSuffix("%");

    _clusterColorOptionAction.setSerializationName("MapColoringAction");
    _selectedIdAction.setSerializationName("SelectedIdAction");
    _colorMapAction.setSerializationName("ColorMapction");
    _sizeAction.setSerializationName("PointSizeAction");
    _opacityAction.setSerializationName("OpacityAction");
}

void SharedSettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _clusterColorOptionAction.fromParentVariantMap(variantMap);
    _selectedIdAction.fromParentVariantMap(variantMap);
    _colorMapAction.fromParentVariantMap(variantMap);
    _sizeAction.fromParentVariantMap(variantMap);
    _opacityAction.fromParentVariantMap(variantMap);
}

QVariantMap SharedSettingsAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _clusterColorOptionAction.insertIntoVariantMap(variantMap);
    _selectedIdAction.insertIntoVariantMap(variantMap);
    _colorMapAction.insertIntoVariantMap(variantMap);
    _sizeAction.insertIntoVariantMap(variantMap);
    _opacityAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

SharedSettingsAction::Widget::Widget(QWidget* parent, SharedSettingsAction* sharedSettingsAction) :
    WidgetActionWidget(parent, sharedSettingsAction)
{
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    auto layout = new QGridLayout();

    const int margin = 5;
    layout->setContentsMargins(margin, margin, margin, margin);
    layout->setSpacing(2);

    layout->setRowStretch(0, 50);

    layout->addWidget(sharedSettingsAction->getClusterColorOptionAction().createLabelWidget(this), 0, 0);
    layout->addWidget(sharedSettingsAction->getClusterColorOptionAction().createWidget(this), 0, 1);
    layout->addWidget(sharedSettingsAction->getSelectedIdAction().createWidget(this), 0, 2);
    layout->addWidget(sharedSettingsAction->getColorMapAction().createWidget(this), 0, 3);

    layout->setRowStretch(1, 50);

    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->addWidget(sharedSettingsAction->getSizeAction().createLabelWidget(this));
    horizontalLayout->addWidget(sharedSettingsAction->getSizeAction().createWidget(this, DecimalAction::Slider));
    horizontalLayout->addWidget(sharedSettingsAction->getOpacityAction().createLabelWidget(this));
    horizontalLayout->addWidget(sharedSettingsAction->getOpacityAction().createWidget(this, DecimalAction::Slider));

    layout->addLayout(horizontalLayout, 1, 0, 1, 8);

    for (const int row : {0, 1, 2, 3, 4, 5})
    {
        layout->setColumnStretch(row, 1);
        layout->setColumnMinimumWidth(row, 100);
    }

    layout->setColumnStretch(6, 99);

    setLayout(layout);

}