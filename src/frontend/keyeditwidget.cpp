/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "keyeditwidget.h"
#include <qevent.h>
#include <QDebug>

KeyEditWidget::KeyEditWidget(QWidget* parent): QLineEdit(parent)
{
	key = (Qt::Key)0;
}


void KeyEditWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat())
		return;

	if (event->key() == Qt::Key_Escape)
	{
		return;
	}

	setKey((Qt::Key)event->key());
}
void KeyEditWidget::setKey(Qt::Key key)
{
	this->key = (Qt::Key)key;
	this->setKeyText();
}

void KeyEditWidget::setKeyText()
{
	if ((int)this->key > 32 && (int)this->key < 512)
	{
		// show as ascii code
		char key_ascii = (char)key;
		this->setText(QString::fromUtf16((ushort*)&key, 1));
		return;
	}

	
	switch(this->key)
	{
		case Qt::Key_Shift:
			setText("Shift");
			break;
		case Qt::Key_Up:
			setText("Arrow UP");
			break;
		case Qt::Key_Down:
			setText("Arrow DOWN");
			break;
		case Qt::Key_Left:
			setText("Arrow LEFt");
			break;
		case Qt::Key_Right:
			setText("Arrow RIGHT");
			break;
		case Qt::Key_Alt:
			setText("Alt");
			break;
		case Qt::Key_AltGr:
			setText("AltGr");
			break;
		case Qt::Key_Control:
			setText("Ctrl");
			break;
		case Qt::Key_Home:
			setText("Home");
			break;
		case Qt::Key_End:
			setText("End");
			break;
		case Qt::Key_PageDown:
			setText("Page Down");
			break;
		case Qt::Key_PageUp:
			setText("Page Up");
			break;
		case Qt::Key_Insert:
			setText("Insert");
			break;
		case Qt::Key_Delete:
			setText("Delete");
			break;
		case Qt::Key_Backspace:
			setText("Backspace");
			break;
		default:
			this->setText(QString::number(key));
			break;
	}
}



