#include "CrossSpeciesViewerPlugin.h"

// Local includes
#include "SelectedDatasetsAction.h"

// HDPS includes
#include "PointData/PointData.h"
#include <QDebug>
#include "ClusterData/ClusterData.h"
#include "event/Event.h"


#include <actions/PluginTriggerAction.h>
#include <actions/WidgetAction.h>

// QT includes

#include <QMimeData>


#include <iostream>
#include <cassert>
#include <set>
#include <algorithm>


#ifdef __cpp_lib_parallel_algorithm
#include <execution>
#endif

#include <omp.h>






Q_PLUGIN_METADATA(IID "nl.BioVault.CrossSpeciesViewerPlugin")
//Q_DECLARE_METATYPE(QWidget*)
using namespace hdps;
using namespace hdps::gui;
using namespace hdps::plugin;
using namespace hdps::util;

namespace 
{
    namespace local
    {

        template<typename T>
        bool is_exact_type(const QVariant& variant)
        {
            auto variantType = variant.metaType();
            auto requestedType = QMetaType::fromType<T>();
            return (variantType == requestedType);
        }
        template<typename T>
        T get_strict_value(const QVariant& variant)
        {
            if (is_exact_type<T>(variant))
                return variant.value<T>();
            else
            {
#ifdef _DEBUG
                qDebug() << "Error: requested " << QMetaType::fromType<T>().name() << " but value is of type " << variant.metaType().name();
#endif
                return T();
            }
        }

        


        QString fromCamelCase(const QString& s, QChar c = '_') {

            static QRegularExpression regExp1{ "(.)([A-Z][a-z]+)" };
            static QRegularExpression regExp2{ "([a-z0-9])([A-Z])" };

            QString result = s;

            QString s2("\\1");
            s2 += QString(c);
            s2 += "\\2";
            result.replace(regExp1, s2);
            result.replace(regExp2, s2);

            return result.toLower();

        }

        QString toCamelCase(const QString& s, QChar c = '_') {

            QStringList parts = s.split(c, Qt::SkipEmptyParts);
            for (int i = 1; i < parts.size(); ++i)
                parts[i].replace(0, 1, parts[i][0].toUpper());

            return parts.join("");

        }

        QStringList get_stringlist(QStandardItem* item)
        {
            QStringList result;
            if (item->hasChildren())
            {
                const int nrOfChilden = item->rowCount();
                for (int c = 0; c < nrOfChilden; ++c)
                {
                    result.append(get_stringlist(item->child(c, 0)));
                }
            }
            else
            {
                result << item->text();
            }
            return result;
        }


        void setDatabaseFilterFunction(DatasetPickerAction &action, QString parentName, DataType dataType)
        {
            action.setDatasetsFilterFunction([parentName, dataType](const hdps::Datasets& datasets) -> Datasets {
                Datasets result;

                for (auto dataset : datasets)
                {
                    if(dataset->getGuiName() == parentName)
                    {
                        auto children = dataset->getChildren(dataType);
                        for(auto child : children)
                                result << child;
                    }
                    else
                    {
                        if (dataset->getGuiName().contains("Cluster Differential Expression 1::SelectedIDMeanExpressionsDataset"))
                            result << dataset;
                    }
                    

                }

                return result;
                });
        }
    }


    
}

namespace CytosploreViewerPlugin
{
    CrossSpeciesViewerPlugin::CrossSpeciesViewerPlugin(const hdps::plugin::PluginFactory* factory)
        : ViewPlugin(factory)
        , _originalName(getGuiName())
        , _pointsDatasetsAction(this, 3, "PointDatasets", "Point Datasets")
        , _colorDatasetsAction(this, 3, "ColorDatasets", "Color Datasets")
        , _settingsAction(this)
        , _colorMapAction(this, "Color map")
        , _sizeAction(this, "Point Size", 0.0, 100.0, 10, 10)
        , _opacityAction(this, "Opacity", 0.0, 100.0, 50.0, 50.0)
        , _clusterColorOptionAction(this, "Map Coloring")
        //, _tsneMapOptionAction(this, "Select tSNE map")
       // , _dimensionXPickerAction(this, "X")
       // , _dimensionYPickerAction(this, "Y")
        , _selectionColorAction(this, "Selection Color")
        , _selectedIdAction(this, "SelectedId")
        , _selectedIdActionWidget(nullptr)
        , _colorMapWidget(nullptr)
    {
        setSerializationName(getGuiName());


      //  local::setDatabaseFilterFunction(_pointsDatasetsAction.data(HUMAN)->datasetPickerAction, "Human", PointType);
     //   local::setDatabaseFilterFunction(_pointsDatasetsAction.data(MARMOSET)->datasetPickerAction, "Marmoset", PointType);
     //   local::setDatabaseFilterFunction(_pointsDatasetsAction.data(MOUSE)->datasetPickerAction, "Mouse", PointType);
     //   _pointsDatasetsAction.data(HUMAN)->datasetPickerAction.setCurrentText("NumericalMetaData");
     //   _pointsDatasetsAction.data(MARMOSET)->datasetPickerAction.setCurrentText("NumericalMetaData");
     //   _pointsDatasetsAction.data(MOUSE)->datasetPickerAction.setCurrentText("NumericalMetaData");

        local::setDatabaseFilterFunction(_colorDatasetsAction.data(HUMAN)->datasetPickerAction, "Human", ClusterType);
        local::setDatabaseFilterFunction(_colorDatasetsAction.data(MARMOSET)->datasetPickerAction, "Marmoset", ClusterType);
        local::setDatabaseFilterFunction(_colorDatasetsAction.data(MOUSE)->datasetPickerAction, "Mouse", ClusterType);

        
        _settingsAction.addAction(_selectionColorAction, 10);
        _settingsAction.addAction(_pointsDatasetsAction, 1);
        _settingsAction.addAction(_colorDatasetsAction, 1);

        _sizeAction.setSuffix("px");
        _opacityAction.setSuffix("%");

        

        _clusterColorOptionAction.setOptions({ "Class","Subclass","Cross-species Cluster", "Mean Expressions"});
        connect(&_clusterColorOptionAction, &OptionAction::currentTextChanged, this, &CrossSpeciesViewerPlugin::mapColoringChanged);

        /*
        _tsneMapOptionAction.setOptions({ "Overview", "GABAergic", "Glutamatergic", "Non-Neuronal" });
        connect(&_tsneMapOptionAction, &OptionAction::currentTextChanged, this, &CrossSpeciesViewerPlugin::mapSelectionChanged);

        _dimensionXPickerAction.setPointsDataset(_pointsDatasetsAction.data(HUMAN)->currentDataset);
        _dimensionYPickerAction.setPointsDataset(_pointsDatasetsAction.data(HUMAN)->currentDataset);
        */
        serializeAction(&_pointsDatasetsAction);
        serializeAction(&_colorDatasetsAction);
        serializeAction(&_settingsAction);
        serializeAction(&_colorMapAction);
        serializeAction(&_sizeAction);
        serializeAction(&_opacityAction);
        serializeAction(&_clusterColorOptionAction);
//        serializeAction(&_tsneMapOptionAction);
//        serializeAction(&_dimensionXPickerAction);
//        serializeAction(&_dimensionYPickerAction);
        serializeAction(&_selectionColorAction);
        serializeAction(&_selectedIdAction);
        
    }

    QString CrossSpeciesViewerPlugin::getOriginalName() const
    {
        return _originalName;
    }


    void CrossSpeciesViewerPlugin::init()
    {
        QWidget& mainWidget = getWidget();
        auto mainLayout = new QGridLayout();
        delete mainWidget.layout();
        mainWidget.setLayout(mainLayout);

       // mainWidget.setAcceptDrops(true);
        mainWidget.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

        const int margin = 5;
        mainLayout->setContentsMargins(margin, margin, margin, margin);
        mainLayout->setSpacing(2);

        
        
        auto *widget = _settingsAction.createWidget(&mainWidget);
       // widget->hide();
        int row = 0;
        int column = 0;
        mainLayout->addWidget(widget, row, 0, 1, 10);
        mainLayout->setRowStretch(row, 1);

        /*
        row++;
        
        mainLayout->addWidget(_tsneMapOptionAction.createLabelWidget(&mainWidget), row, column++);
        mainLayout->addWidget(_tsneMapOptionAction.createWidget(&mainWidget), row, column++);
        mainLayout->setRowStretch(row, 50);
        */
        row++;
        column = 0;
        mainLayout->addWidget(_clusterColorOptionAction.createLabelWidget(&mainWidget), row, column++);
        mainLayout->addWidget(_clusterColorOptionAction.createWidget(&mainWidget), row, column++);
        _selectedIdActionWidget = _selectedIdAction.createWidget(&mainWidget);
        mainLayout->addWidget(_selectedIdActionWidget, row, column++);
        mainLayout->setRowStretch(row, 50);
        _colorMapWidget = _colorMapAction.createWidget(&mainWidget, ColorMapAction::EditRange);
        mainLayout->addWidget(_colorMapWidget, row, column++);
       
        
        mainLayout->setRowStretch(row, 50);

        row++;
        column = 0;
        mainLayout->addWidget(_sizeAction.createLabelWidget(&mainWidget), row, column++);

    	QHBoxLayout* horizontalLayout = new QHBoxLayout(&mainWidget);
        horizontalLayout->addWidget(_sizeAction.createWidget(&mainWidget, DecimalAction::Slider));
        horizontalLayout->addWidget(_opacityAction.createLabelWidget(&mainWidget));
        horizontalLayout->addWidget(_opacityAction.createWidget(&mainWidget, DecimalAction::Slider));
        mainLayout->addLayout(horizontalLayout, row++, column, 1, 8);
        mainLayout->setRowStretch(row, 50);
        
        mainLayout->setColumnStretch(0, 1);
        mainLayout->setColumnMinimumWidth(0, 100);
        mainLayout->setColumnStretch(1, 1);
        mainLayout->setColumnMinimumWidth(1, 100);
        mainLayout->setColumnStretch(2, 1);
        mainLayout->setColumnMinimumWidth(2, 100);
        mainLayout->setColumnStretch(3, 1);
        mainLayout->setColumnMinimumWidth(3, 100);
        mainLayout->setColumnStretch(4, 1);
        mainLayout->setColumnMinimumWidth(4, 100);
        mainLayout->setColumnStretch(5, 1);
        mainLayout->setColumnMinimumWidth(5, 100);
        mainLayout->setColumnStretch(6, 99);
        
    }


    void CrossSpeciesViewerPlugin::onDataEvent(hdps::DataEvent* dataEvent)
    {
        // Event which gets triggered when a dataset is added to the system.
        if (dataEvent->getType() == EventType::DataAdded)
        {
            //    _differentialExpressionWidget->addDataOption(dataEvent->getDataset()->getGuiName());
        }
        // Event which gets triggered when the data contained in a dataset changes.
        if (dataEvent->getType() == EventType::DataChanged)
        {
            //dataEvent->getDataset()
        }
    }

   


    void CrossSpeciesViewerPlugin::fromVariantMap(const QVariantMap& variantMap)
    {
        
        _pointsDatasetsAction.data(HUMAN)->datasetPickerAction.connectToPublicActionByName("ViewerScatterplot 1::Embedding");
        _colorDatasetsAction.data(HUMAN)->datasetPickerAction.connectToPublicActionByName("ViewerScatterplot 1::Color");

        _pointsDatasetsAction.data(MARMOSET)->datasetPickerAction.connectToPublicActionByName("ViewerScatterplot 2::Embedding");
        _colorDatasetsAction.data(MARMOSET)->datasetPickerAction.connectToPublicActionByName("ViewerScatterplot 2::Color");

        _pointsDatasetsAction.data(MOUSE)->datasetPickerAction.connectToPublicActionByName("ViewerScatterplot 3::Embedding");
        _colorDatasetsAction.data(MOUSE)->datasetPickerAction.connectToPublicActionByName("ViewerScatterplot 3::Color");



        const auto globalPointSizeName = "ViewerScatterplot::GlobalPointSize";
        const auto globalPointOpacityName = "ViewerScatterplot::GlobalPointOpacity";
        _sizeAction.connectToPublicActionByName(globalPointSizeName);
        _opacityAction.connectToPublicActionByName(globalPointOpacityName);

        _colorMapAction.connectToPublicActionByName("ViewerScatterplot::GlobalColorMap");

        //_dimensionXPickerAction.connectToPublicActionByName("GlobalDimensionX");
       // _dimensionYPickerAction.connectToPublicActionByName("GlobalDimensionY");
        _selectionColorAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::ConnectViaApi);
        _selectionColorAction.connectToPublicActionByName("ViewerScatterplot::GlobalSelectionColor");

        _selectedIdAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::ConnectViaApi);
        _selectedIdAction.connectToPublicActionByName("Cluster Differential Expression 1::LastSelectedId");
        connect(&_selectedIdAction, &StringAction::stringChanged, this, [this](const QString& dummy)->void {this->_clusterColorOptionAction.setCurrentText("Mean Expressions"); });

        /*
        _rightSideDEColumnAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::ConnectViaApi);
        _rightSideDEColumnAction.connectToPublicActionByName("Cluster Differential Expression 1::TableViewRightSideInfo");
        QVariantMap emptColumn;
        emptColumn["test"] = QVariantMap();
        _rightSideDEColumnAction.setVariant(emptColumn);
        */

        ViewPlugin::fromVariantMap(variantMap);
        auto version = variantMap.value("CrossSpeciesViewerPluginVersion", QVariant::fromValue(uint(0))).toUInt();
        if (version > 0)
        {
            for (auto action : _serializedActions)
            {
               
                if (variantMap.contains(action->getSerializationName()))
                    action->fromParentVariantMap(variantMap);

            }
        }

        for(qsizetype i=0; i < _colorDatasetsAction.size(); ++i)
			_colorDatasetsAction.data(i)->datasetPickerAction.setCurrentText("class");

        _clusterColorOptionAction.setCurrentText("Class");
    }

    QVariantMap CrossSpeciesViewerPlugin::toVariantMap() const
    {
        QVariantMap variantMap = ViewPlugin::toVariantMap();
        variantMap["CrossSpeciesViewerPluginVersion"] = 2;
        for (auto action : _serializedActions)
        {
            assert(action->getSerializationName() != "#Properties");
            action->insertIntoVariantMap(variantMap);
        }
        
        return variantMap;

    }

    void CrossSpeciesViewerPlugin::serializeAction(WidgetAction* w)
    {
        assert(w != nullptr);
        if (w == nullptr)
            return;
        QString name = w->text();
        assert(!name.isEmpty());
        QString apiName = local::toCamelCase(name, ' ');
       // w->setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
        w->setSerializationName(apiName);
    	_serializedActions.append(w);
    }

    void CrossSpeciesViewerPlugin::publishAndSerializeAction(WidgetAction* w, bool serialize)
    {
        assert(w != nullptr);
        if (w == nullptr)
            return;
        QString name = w->text();
        assert(!name.isEmpty());
        QString apiName = local::toCamelCase(name, ' ');
        w->setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
        w->publish(_originalName + "::" + apiName);
        w->setSerializationName(apiName);
        if (serialize)
            _serializedActions.append(w);
    }


    void CrossSpeciesViewerPlugin::mapColoringChanged(const QString& option)
    {
        QString label = "";
        bool cluster_label = true;
        if (option == "Class")
        {
            label = "class";
        }
        else if (option == "Subclass")
        {
            label = "subclass";
        }
        else if (option == "Cross-species Cluster")
        {
            label = "cross_species_cluster";
        }
        else
        {
            cluster_label = false;
            label = "Cluster Differential Expression 1::SelectedIDMeanExpressionsDataset ";
            
        }
        for (qsizetype i = 0; i < _colorDatasetsAction.size(); ++i)
        {
            if (cluster_label)
            {
                _colorDatasetsAction.data(i)->datasetPickerAction.setCurrentText(label);
                _selectedIdActionWidget->hide();
                _colorMapWidget->hide();
            }
            else
            {
                QString specific_label = label + QString::number(i);
                qDebug() << "mapColoringChanged " << specific_label;
                _colorDatasetsAction.data(i)->datasetPickerAction.setCurrentText(specific_label);
                _selectedIdActionWidget->show();
                _colorMapWidget->show();
            }
        }
        
	    
    }
    void CrossSpeciesViewerPlugin::mapSelectionChanged(const QString& label)
    {
        QString x_label = "_" + label + "_X";
        QString y_label = "_" + label + "_Y";
//        _dimensionXPickerAction.setCurrentDimensionName(x_label);
//        _dimensionYPickerAction.setCurrentDimensionName(y_label);
        

    }

    

    namespace  local
    {
	    void get_recursive_cluster_tree(QStandardItem *item, Dataset<hdps::DatasetImpl> currentDataset, const QVector<QString> &hierarchy, qsizetype h,  bool firstTime, bool intersection = true, const std::vector<uint32_t> &indices = {})
	    {
            if(h >= hierarchy.size())
                return;
            
            auto childDatasets = currentDataset->getChildren({ ClusterType });
            for (qsizetype c=0; c < childDatasets.size(); ++c)
            {
                if (childDatasets[c]->getGuiName() == hierarchy[h])
                {
                    hdps::Dataset<Clusters> clusterData = childDatasets[c];
                    auto clusters = clusterData->getClusters();

                   
                    if(intersection && !firstTime)
                    {
                        QSet<QString> clusterNames;
                        for (auto cluster : clusters)
                        {
                            QString name = cluster.getName();
                            clusterNames.insert(name);
                        }
                        for (qsizetype row = item->rowCount()-1; row >=0; --row)
                        {
                            QStandardItem* child = item->child(row, 0);
                            if(!clusterNames.contains(child->text()))
                            {
                                item->removeRow(row);
                            }
                        }
                    }
                    
                    for (auto cluster : clusters)
                    {
                        QString name = cluster.getName();
                        QStandardItem* correspondingItem = nullptr;
                        
                        if(!firstTime)
                        {
                           for(qsizetype row = 0; row < item->rowCount(); ++row)
                           {
                               QStandardItem* child = item->child(row, 0);
                               if (child->text() == name)
                               {
                                   correspondingItem = child;
									break;
                               }
                           }
                           if (intersection && (correspondingItem == nullptr))
                               continue;;
                        }

                        
                        
                        {
                            std::vector<uint32_t> clusterIndices = cluster.getIndices();
                            std::sort(clusterIndices.begin(), clusterIndices.end());
                            std::vector<uint32_t> intersectingIndices;
                            if (h == 0)
                                intersectingIndices = clusterIndices;
                            else
                            {
                                std::set_intersection(indices.cbegin(), indices.cend(), clusterIndices.cbegin(), clusterIndices.cend(), std::back_inserter(intersectingIndices));
                            }
                            if (!intersectingIndices.empty())
                            {
                               if(correspondingItem == nullptr)
                               {
                                   QPixmap pixmap(16, 16);
                                   pixmap.fill(cluster.getColor());
                                   correspondingItem = new QStandardItem(pixmap,name);
                                   correspondingItem->setData(h, Qt::UserRole);
                                   item->appendRow(correspondingItem);
                               }
                            	get_recursive_cluster_tree(correspondingItem, currentDataset, hierarchy, h + 1, firstTime, intersection, intersectingIndices);
                            }
                     
                        }
                    }
                    break;
                }
            }
	    }
    }
    
    QIcon CrossSpeciesViewerFactory::getIcon(const QColor& color /*= Qt::black*/) const
    {
        return Application::getIconFont("FontAwesome").getIcon("table", color);
    }

    CrossSpeciesViewerPlugin* CrossSpeciesViewerFactory::produce()
    {
        return new CrossSpeciesViewerPlugin(this);
    }

    hdps::DataTypes CrossSpeciesViewerFactory::supportedDataTypes() const
    {
        DataTypes supportedTypes;
        supportedTypes.append(ClusterType);
        return supportedTypes;
    }

    hdps::gui::PluginTriggerActions CrossSpeciesViewerFactory::getPluginTriggerActions(const hdps::Datasets& datasets) const
    {

        PluginTriggerActions pluginTriggerActions;


        return pluginTriggerActions;
    }


}
