#pragma once



// Local includes

#include "SelectedDatasetsAction.h"

#include "SettingsAction.h"

// HDPS includes
#include <ViewPlugin.h>
#include <Dataset.h>

#include "actions/Actions.h"
#include "actions/VariantAction.h"

#include "PointData/DimensionPickerAction.h"



using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;


class Points;
class Clusters;


namespace hdps {
    namespace gui {
        class DropWidget;
    }
}

namespace  CytosploreViewerPlugin
{
    class CrossSpeciesViewerPlugin : public ViewPlugin
    {
        Q_OBJECT

    	typedef std::pair<QString, std::vector<ptrdiff_t>> DimensionNameMatch;

    	enum { HUMAN, MARMOSET, MOUSE };

    public:
        CrossSpeciesViewerPlugin(const hdps::plugin::PluginFactory* factory);

        QString getOriginalName() const;

        void init() override;

        void onDataEvent(hdps::DataEvent* dataEvent);

       

        void addConfigurableWidget(const QString& name, QWidget* widget);
        QWidget* getConfigurableWidget(const QString& name);

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

        void serializeAction(WidgetAction* w);
        void publishAndSerializeAction(WidgetAction* w, bool serialize = true);


       

    protected slots:

        void mapColoringChanged(const QString& label);
        void mapSelectionChanged(const QString& label);

    private:
        const QString                        _originalName;
        QVector<WidgetAction*>               _serializedActions;
        SelectedDatasetsAction               _pointsDatasetsAction;
        SelectedDatasetsAction               _colorDatasetsAction;
        SettingsAction                       _settingsAction;
        ColorMapAction                       _colorMapAction;
        DecimalAction                        _sizeAction;                /** Point size action */
        DecimalAction                        _opacityAction; /** Point opacity action */
        OptionAction                         _clusterColorOptionAction;
        //OptionAction                         _tsneMapOptionAction;
       // DimensionPickerAction                _dimensionXPickerAction;
       // DimensionPickerAction                _dimensionYPickerAction;
        ColorAction                          _selectionColorAction;
        StringAction                         _selectedIdAction;
        QWidget*                             _selectedIdActionWidget;
        QWidget*                             _colorMapWidget;
       
    };



    class CrossSpeciesViewerFactory : public ViewPluginFactory
    {
        Q_OBJECT
            Q_PLUGIN_METADATA(IID   "nl.BioVault.CrossSpeciesViewerPlugin"
                FILE  "CrossSpeciesViewerPlugin.json")
            Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)

    public:
        CrossSpeciesViewerFactory() {}
        ~CrossSpeciesViewerFactory() override {}

        /** Returns the plugin icon */
        QIcon getIcon(const QColor& color = Qt::black) const override;

        CrossSpeciesViewerPlugin* produce() override;

        hdps::DataTypes supportedDataTypes() const override;


        hdps::gui::PluginTriggerActions getPluginTriggerActions(const hdps::Datasets& datasets) const override;

        
    };
}
