/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <qwidget.h>
#include "ui_menuwidget_ui.h"

class PlayerHandler;
class AddPlayerDialog;
class QStandardItemModel;
class GameInfo;
namespace ABomberman {
class Map;
class GameField;
}

/**
 * MenuWidget provides controls to setup map, players and game properties.
 *
 */
class MenuWidget : public QWidget, public Ui::MenuWidget
{
	Q_OBJECT
	void initMapList();
    QStandardItemModel *_playermodel;

public:
	MenuWidget(QWidget* parent);

	/// Finds PlayerHandler in configured players, which have binded given key and returns pointer to it.
	/// if noone found, returns NULL
	PlayerHandler *whoBindKey(Qt::Key key);
	
public slots:
    void playClicked(bool);
    void clickedQuit(bool);
    void clickedAddPlayer(bool);
    void clickedRemovePlayer(bool);
	void clickedEditPlayer(bool);
    void createPlayer(AddPlayerDialog*);
    void changePlayer(AddPlayerDialog*);
signals:
	void play(GameInfo* settings);
};

#endif // MENUWIDGET_H
