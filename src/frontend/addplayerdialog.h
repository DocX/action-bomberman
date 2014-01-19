/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ADDPLAYERDIALOG_H
#define ADDPLAYERDIALOG_H

#include <qdialog.h>
#include "ui_addplayerdialog_ui.h"

class MenuWidget;
class PlayerHandler;
class AddPlayerDialog: public QDialog, public Ui::AddPlayerDialog
{
	Q_OBJECT

	void setKeysEnabled(bool);
	MenuWidget *menu;
	
public:
    AddPlayerDialog(MenuWidget* parent = 0, Qt::WindowFlags f = 0);

	PlayerHandler *edited_player;
	void setPlayerByDialog(PlayerHandler *player);
	
public slots:
    void toggledAi(bool);
    void playerAdd(bool);
    void predefinedChanged(int);

signals:
	void playerAdded(AddPlayerDialog* dialog);

};

#endif // ADDPLAYERDIALOG_H
