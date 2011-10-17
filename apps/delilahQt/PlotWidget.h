#include <QtGui>
#include <iostream>
#include <KDChartChart>
#include <KDChartBarDiagram>

// Include header files for application components.
// ...


#include <assert.h>

class PlotWidget : public QWidget
{
    Q_OBJECT
	
	KDChart::Chart m_chart;
    QStandardItemModel m_model;
    
public:
    
    PlotWidget(QWidget *parent = 0);
    
private slots:
    
    
};

