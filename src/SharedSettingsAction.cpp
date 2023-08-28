#include "SharedSettingsAction.h"

using namespace hdps;
using namespace hdps::gui;

SharedSettingsAction::SharedSettingsAction(QObject* parent) :
    WidgetAction(parent, "Shared Settings"),
    _clusterColorOptionAction(this, "Color by", { "Constant", "Scatter layout", "Numerical MetaData", "Data/class", "Data/subclass", "Data/cross_species_cluster", "Mean Expressions" }),
    _selectedGeneNameAction(this, "Selected Dimension"),
    _colorMapAction(this, "Color Map"),
    _sizeAction(this, "Point Size", 0.0, 100.0, 10, 10),
    _opacityAction(this, "Opacity", 0.0, 100.0, 50.0, 50.0),
    _datasetPickerAction(this, "Dataset")
{
    setText("Settings");
    setSerializationName("Settings");

    _sizeAction.setSuffix("px");
    _opacityAction.setSuffix("%");

    _selectedGeneNameAction.setEnabled(false);

    _clusterColorOptionAction.setSerializationName("MapColoringAction");
    _selectedGeneNameAction.setSerializationName("SelectedGeneNameAction");
    _colorMapAction.setSerializationName("ColorMapction");
    _sizeAction.setSerializationName("PointSizeAction");
    _opacityAction.setSerializationName("OpacityAction");

    _datasetPickerAction.setSerializationName("DatasetPickerAction");

    connect(&_clusterColorOptionAction, &OptionAction::currentTextChanged, this, [this](const QString& s) {

        if (s == "Mean Expressions")
            _selectedGeneNameAction.setEnabled(true);
        else
            _selectedGeneNameAction.setEnabled(false);

        });
}

void SharedSettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _clusterColorOptionAction.fromParentVariantMap(variantMap);
    _selectedGeneNameAction.fromParentVariantMap(variantMap);
    _colorMapAction.fromParentVariantMap(variantMap);
    _sizeAction.fromParentVariantMap(variantMap);
    _opacityAction.fromParentVariantMap(variantMap);
    _datasetPickerAction.fromParentVariantMap(variantMap);
}

QVariantMap SharedSettingsAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _clusterColorOptionAction.insertIntoVariantMap(variantMap);
    _selectedGeneNameAction.insertIntoVariantMap(variantMap);
    _colorMapAction.insertIntoVariantMap(variantMap);
    _sizeAction.insertIntoVariantMap(variantMap);
    _opacityAction.insertIntoVariantMap(variantMap);
    _datasetPickerAction.insertIntoVariantMap(variantMap);

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

    layout->addWidget(sharedSettingsAction->getClusterColorOptionAction().createLabelWidget(this), 0, 0);
    layout->addWidget(sharedSettingsAction->getClusterColorOptionAction().createWidget(this), 0, 1);
    layout->addWidget(sharedSettingsAction->getSelectedIdAction().createLabelWidget(this), 0, 2);
    layout->addWidget(sharedSettingsAction->getSelectedIdAction().createWidget(this), 0, 3);
    layout->addWidget(sharedSettingsAction->getColorMapAction().createLabelWidget(this), 0, 4);
    layout->addWidget(sharedSettingsAction->getColorMapAction().createWidget(this), 0, 5);

    layout->addWidget(sharedSettingsAction->getSizeAction().createLabelWidget(this), 1, 0);
    layout->addWidget(sharedSettingsAction->getSizeAction().createWidget(this, DecimalAction::Slider), 1, 1);
    layout->addWidget(sharedSettingsAction->getOpacityAction().createLabelWidget(this), 1, 2);
    layout->addWidget(sharedSettingsAction->getOpacityAction().createWidget(this, DecimalAction::Slider), 1, 3);

    setLayout(layout);
}