/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "playeritem.h"
#include "playerhandler.h"

PlayerItem::PlayerItem(PlayerHandler* player): QStandardItem()
{
	setPlayerHandler(player);
}

PlayerHandler* PlayerItem::playerHandler()
{
	return _player;
}
void PlayerItem::setPlayerHandler(PlayerHandler* player)
{
	if (player != NULL)
		this->setText(player->name());
	this->_player = player;
}

