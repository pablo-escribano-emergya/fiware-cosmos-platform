/*
 * FILE:		LoadExistingQueueDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include <iostream>

#include "LoadExistingQueueDlg.h"
#include "DelilahQtApp.h"
#include "Queue.h"
#include "Misc.h"


void LoadExistingQueueDlg::initNameSelectionWidget()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;
	QList<Queue*> queues = app->getQueues();

	QStringList names;
	for(int i=0; i<queues.size(); i++ )
		names << queues.at(i)->getName();

	arrangeNamesInTreeWidget(ui.nameTreeWidget, name_column_number, names);
	if(ui.nameTreeWidget->topLevelItemCount()>0)
		ui.nameTreeWidget->setCurrentItem(ui.nameTreeWidget->topLevelItem(0));
}
