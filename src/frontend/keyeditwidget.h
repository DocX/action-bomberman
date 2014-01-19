/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef KEYEDITWIDGET_H
#define KEYEDITWIDGET_H

#include <QLineEdit>


class KeyEditWidget : public QLineEdit
{

	void setKeyText();
	
public:
    KeyEditWidget(QWidget* parent = 0);
	Qt::Key key;
	void setKey(Qt::Key key);
	
protected:
    virtual void keyPressEvent(QKeyEvent* );
};

#endif // KEYEDITWIDGET_H
