#ifndef LOG_TAB_H
#define LOG_TAB_H

#include <QObject>
#include <QWidget>



/* ****************************************************************************
*
* LogTab -
*/
class LogTab : public QWidget
{
	Q_OBJECT

public:
	LogTab(const char* name, QWidget *parent = 0);
};

#endif
