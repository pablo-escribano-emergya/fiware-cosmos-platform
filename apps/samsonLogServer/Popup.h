#ifndef POPUP_H
#define POPUP_H

/* ****************************************************************************
*
* FILE                     Popup.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 21 2011
*
*/
#include <QDialog>



/* ****************************************************************************
*
* Popup - 
*/
class Popup : public QDialog
{
public:
	Popup(char* title, char* text);
};

#endif
