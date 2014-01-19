/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef PLAYERITEM_H
#define PLAYERITEM_H

#include <QStandardItem>

class PlayerHandler;
class PlayerItem : public QStandardItem
{
	PlayerHandler *_player;

public:
    PlayerItem(PlayerHandler *player);

	void setPlayerHandler(PlayerHandler* player);
    PlayerHandler* playerHandler();
};

#endif // PLAYERITEM_H
