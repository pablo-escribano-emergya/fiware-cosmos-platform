#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*

#include "LogTab.h"             // Own interface



/* ****************************************************************************
*
* LogTab::LogTab - 
*/
LogTab::LogTab(const char* name, QWidget *parent) : QWidget(parent)
{
	QLabel*      nameLabel   = new QLabel(tr(name));
	QVBoxLayout* mainLayout  = new QVBoxLayout;

	mainLayout->addWidget(nameLabel);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
