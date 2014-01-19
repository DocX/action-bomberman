/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "gameobject.h"
#include "gamefield.h"
#include <QDate>
#include "maptile.h"
#include <qrect.h>


using namespace ABomberman;

ABomberman::GameObject::GameObject(GameField* parentField)
{
	
	this->direction = NORTH;
	//this->currentSpeed = 0.0;
	this->positionX = 0;
	this->positionY = 0;
	
	
	type = GameObject::UNSPECIFIED;
	
	addToField(parentField);
}

void GameObject::addToField(GameField* parentField)
{
	this->field = parentField;
	if (field != NULL)
	{
		object_id = field->insertObject(this);
		lastUpdateFieldTime = field->elapsedPlayTime();
	}
}


quint32 GameObject::objectID()
{
	return object_id;
}

void GameObject::setPosition(MapCoord pos)
{
	positionX = pos.col * MapTile::SIDESIZE;
	positionY = pos.row * MapTile::SIDESIZE;
}

MapTile* GameObject::getMapTile()
{
	return this->field->getMap()->getTile(positionX, positionY);
}

QRectF GameObject::objectRect()
{
	return QRectF(this->positionX, this->positionY, MapTile::SIDESIZE, MapTile::SIDESIZE);
}

void GameObject::update()
{
	lastUpdateFieldTime = field->elapsedPlayTime();
}

qint32 GameObject::elapsedFromLastUpdate()
{
	if (field == NULL)
		return 0;
	
	return field->elapsedPlayTime() - lastUpdateFieldTime;
}


void GameObject::react()
{

}

void GameObject::handleMessage(GameObject* sender, GameObject::MessageType type, bool first)
{

}


void GameObject::remove()
{
	this->field->removeObject(this);
}

QPoint GameObject::directionMultipliers()
{
	QPoint p;
	switch(this->direction)
	{
		case EAST:
			p.rx() = 1;
			break;
		case WEST:
			p.rx() = -1;
			break;
		case NORTH:
			p.ry() = -1;
			break;
		case SOUTH:
			p.ry() = 1;
			break;
		default:
			break;
	}

	return p;
}

