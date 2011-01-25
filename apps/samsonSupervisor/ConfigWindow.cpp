/* ****************************************************************************
*
* FILE                     ConfigWindow.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 25 2011
*
*/
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QApplication>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "globals.h"            // networkP, ...
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgAwait.h"        // iomMsgAwait
#include "iomMsgRead.h"         // iomMsgRead
#include "Endpoint.h"           // Endpoint
#include "ConfigWindow.h"       // Own interface



/* ****************************************************************************
*
* ConfigWindow - 
*/
ConfigWindow::ConfigWindow(ss::Endpoint* endpoint)
{
	QGridLayout*              layout;
	QLabel*                   label;
	QSize                     size;
	int                       screenWidth;
	int                       screenHeight;
	int                       x;
	int                       y;
	QDesktopWidget*           desktop = QApplication::desktop();
	QDialogButtonBox*         buttonBox;
	char                      processName[256];
	QCheckBox*                readsBox;
	QCheckBox*                writesBox;
	QCheckBox*                debugBox;
	QCheckBox*                verboseBox;
	QPushButton*              sendButton;  // To be included in QDialogButtonBox
	QLabel*                   traceLevelLabel;
	QListWidget*              traceLevelList;
	QFont                     labelFont("Times", 20, QFont::Normal);
	QFont                     traceFont("Helvetica", 10, QFont::Normal);
	QListWidgetItem*          traceLevelItem[256];
	void*                     dataP = NULL;
	int                       s;
	ss::Message::Header       header;
	ss::Message::MessageCode  code;
	ss::Message::MessageType  type;
	int                       dataLen;

	LM_M(("Here"));
	this->endpoint = endpoint;

	setModal(true);

	layout = new QGridLayout();

	snprintf(processName, sizeof(processName), "%s@%s", endpoint->name.c_str(), endpoint->ip.c_str());
	
	LM_M(("Here"));
	label            = new QLabel(processName);
	buttonBox        = new QDialogButtonBox(QDialogButtonBox::Ok);
	readsBox         = new QCheckBox("Reads");
	writesBox        = new QCheckBox("Writes");
	debugBox         = new QCheckBox("Debug");
	verboseBox       = new QCheckBox("Verbose");
	sendButton       = new QPushButton("Send");
	traceLevelLabel  = new QLabel("Trace Levels");
	traceLevelList   = new QListWidget();

	label->setFont(labelFont);
	traceLevelLabel->setFont(traceFont);
	traceLevelList->setFont(traceFont);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	setWindowTitle("Samson Process Configuration");

	layout->addWidget(label,      0, 1, 1, -1);
	layout->addWidget(readsBox,   1, 0);
	layout->addWidget(writesBox,  2, 0);
	layout->addWidget(debugBox,   3, 0);
	layout->addWidget(verboseBox, 4, 0);
	layout->addWidget(sendButton, 5, 0);
	layout->addWidget(buttonBox,  6, 0);
	layout->addWidget(traceLevelLabel, 1, 1);
	layout->addWidget(traceLevelList,  2, 1);

	memset(traceLevelItem, 0, sizeof(traceLevelItem));
	for (int ix = 0; ix < 256; ix++)
	{
		char* name;
		char  levelName[256];

		name = traceLevelName(ix);
		if (name == NULL)
			continue;

		snprintf(levelName, sizeof(levelName), "%s (%d)", name, ix);
		traceLevelItem[ix] = new QListWidgetItem(levelName);
		traceLevelList->addItem(traceLevelItem[ix]);
		traceLevelItem[ix]->setCheckState(Qt::Checked);
	}

	this->setLayout(layout);
	this->show();

	// Window Geometry
	size = this->size();

	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - size.width())  / 2;
	y = (screenHeight - size.height()) / 2;

	this->move(x, y);

	memset(&header, 0, sizeof(header));

	s = iomMsgSend(endpoint, networkP->me, ss::Message::ConfigGet, ss::Message::Msg);

	if (s != 0)
		LM_E(("iomMsgSend error: %d", s));
	else
	{
		s = iomMsgAwait(endpoint->rFd, 2, 0);
		if (s != 1)
			LM_E(("iomMsgAwait error: %d", s));
		else
		{
			LM_M(("Reading header ..."));
			s = full_read(endpoint->rFd, (char*) &header, sizeof(header));
			if (s != sizeof(header))
				LM_E(("Bad length of header read (read len: %d)", s));
			else
			{
				ss::Message::ConfigData  configData;
				ss::Message::ConfigData* configDataP;
				
				dataP   = &configData;
				dataLen = sizeof(configData);

				s = iomMsgRead(endpoint, &header, &code, &type, &dataP, &dataLen);
				configDataP = (ss::Message::ConfigData*) dataP;
				if (s != 0)
					LM_E(("iomMsgRead returned %d", s));
				else
				{
					// Fill checkboxes and trace levels according to 'dataP' contents
					if (dataP == NULL)
						LM_E(("iomMsgRead didn't fill the data pointer ..."));
					else
					{
						LM_M(("Here"));
						debugBox->setCheckState((configDataP->debug     == true)? Qt::Checked : Qt::Unchecked);
						verboseBox->setCheckState((configDataP->verbose == true)? Qt::Checked : Qt::Unchecked);
						readsBox->setCheckState((configDataP->reads     == true)? Qt::Checked : Qt::Unchecked);
						writesBox->setCheckState((configDataP->writes   == true)? Qt::Checked : Qt::Unchecked);
						
						LM_M(("Here"));
						for (int ix = 0; ix < 256; ix++)
						{
							if (traceLevelItem[ix])
							{
								LM_M(("ix == %d", ix));
								traceLevelItem[ix]->setCheckState((configDataP->traceLevels[ix] == true)? Qt::Checked : Qt::Unchecked);
							}
						}
					}
				}
			}
		}
	}

	this->show();
	LM_M(("Leaving"));
}
