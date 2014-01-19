/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "character.h"
#include <QDebug>
#include <qlinkedlist.h>
#include "maptile.h"
#include "gamefield.h"
#include <qrect.h>

using namespace ABomberman;

ABomberman::Character::Character(qreal initX, qreal initY, GameField* parentField):
	GameObject(parentField)
{
	type = PLAYER;

	positionX = initX;
	positionY = initY;
	
	myBombs = NULL;
	
	reset();
}

void Character::reset()
{
	currentLife = 100;
	bombsCount = 1;
	bombsPower = 2;
	canKick = false;
	shields = 0;

	walkingSpeed = 5;
	currentSpeed = 0;

	delete myBombs;
	myBombs = new QLinkedList<Bomb*>();

	bad_bonus = MapTile::Bonus_None;
	bad_timer = -1;

}


void Character::update()
{
	if (isDead())
		return;
	
    // vypocitat rychlost a smer
    qreal speedX = 0, speedY = 0;
    
	QPoint p = directionMultipliers();
	speedX = currentSpeed * p.x();
	speedY = currentSpeed * p.y();
	
	// align into map grid
	if (currentSpeed > 0)
	{
		if (direction == NORTH || direction == SOUTH)
		{
			qreal dx = getMapTile()->tileRect().x() - positionX;
			positionX += qMin<qreal>(dx * ((dx > 0) ? 1 : -1), currentSpeed) * /*sgn(dx)*/((dx > 0) ? 1 : -1);
		}
		else
		{
			qreal dy = getMapTile()->tileRect().y() - positionY;
			positionY += qMin<qreal>(dy * ((dy > 0) ? 1 : -1), currentSpeed) * /*sgn(dy)*/((dy > 0) ? 1 : -1);
		}
	}
	
	qreal newX, newY;
	newX = positionX + speedX;
	newY = positionY + speedY;
	
	if ((newX != positionX || newY != positionY) && tryMoveTo(newX, newY))
	{
		//qDebug() << "New position is " << newX << newY;
		positionX = newX;
		positionY = newY;
	}

	// countdown bad timer
	if (bad_timer > 0)
	{
		bad_timer -= elapsedFromLastUpdate();
		if (bad_timer <= 0)
		{
			clearBadBonuses();
		}
	}

	GameObject::update();
}

void Character::changeWalk(GameObject::Direction direction, bool state)
{
	this->direction = direction;

	this->currentSpeed = state ? (bad_bonus == MapTile::Bonus_Slowness ? 1 : walkingSpeed ) : 0;
}



void Character::fire()
{
	if (bad_bonus == MapTile::Bonus_WetBombs || isDead() || myBombs->count() == bombsCount)
		return;

	// check if there is already a bomb
	for(int i = 0; i < field->objectsCount(); i++)
	{
		GameObject *obj = field->object(i);
		if (obj->type == GameObject::BOMB && obj->getMapTile() == this->getMapTile())
		{
			//TODO throw if possible
			return;
		}
	}

		
	Bomb *fired_bomb = new Bomb(this, this->getMapTile()->coords(), this->field);
	
	myBombs->append(fired_bomb);
	
	fired_bomb->setState(Bomb::State_Active);
	fired_bomb->setPower(bombsPower);
	fired_bomb->setTimeout(2500);
	fired_bomb->setCountdownActive(true);
}

void Character::handleMessage(ABomberman::GameObject* sender, GameObject::MessageType type, bool first)
{
	if (isDead())
		return;
    switch (type)
	{
		case GameObject::Message_in_explosion:
			if (shields > 0)
			{
				shields--;
				return;
			}
			this->currentLife = 0;
			qDebug() << "Player dead" << currentLife;
			break;
		case GameObject::Message_bomb_detonated:
			//remove if sender is really my
			myBombs->removeOne(static_cast<Bomb*>(sender));
			break;
	}
}


int Character::life()
{
	return currentLife;
}

bool Character::isDead()
{
	return currentLife <= 0;
}

void Character::setBombCount(int arg1)
{
	if (arg1 > 0)
		bombsCount = arg1;
}

void Character::setBombPower(int arg1)
{
	if (arg1 > 0)
		bombsPower = arg1;
}

void Character::setWalkingSpeed(qreal arg1)
{
	if (arg1 > 0)
	{
		walkingSpeed = arg1;
	}
}

bool Character::isWalking()
{
	return currentSpeed >= walkingSpeed;
}


int Character::bombs()
{
	return bombsCount;
}

int Character::power()
{
	return bombsPower;
}

int Character::badBonusReminding()
{
	return bad_timer;
}

int Character::shieldsCount()
{
	return shields;
}

void Character::kickBomb(Bomb *bomb)
{
	// search a bomb in current direction
	// and possible next bomb

	MapCoord my_coord = this->getMapTile()->coords();
	int col_dir = 0;
	int row_dir = 0;
	switch (direction)
	{
		case WEST:
			col_dir = -1;
			break;
		case EAST:
			col_dir = 1;
			break;
		case NORTH:
			row_dir = -1;
			break;
		case SOUTH:
			row_dir = 1;
			break;
	}

	MapCoord bomb_coord = bomb->getMapTile()->coords();
	if (my_coord.col+col_dir == bomb_coord.col && my_coord.row+row_dir == bomb_coord.row)
	{
		bomb->kick(direction, this->walkingSpeed);
	}
	
}


void ABomberman::Character::react()
{
	if (isDead())
		return;
	
    // checks bonuses
    MapTile::Bonus bon = getMapTile()->getBonus();

	switch (bon)
	{
		case MapTile::Bonus_Bomb:
			bombsCount++;
			break;
		case MapTile::Bonus_MorePower:
			bombsPower++;
			break;
		case MapTile::Bonus_CanKick:
			canKick = true;
			break;
/*		case MapTile::Bonus_CanThrow:
			canThrow = true;
			break;
*/		case MapTile::Bonus_Speed:
			if (walkingSpeed < 20) // too much speed is bad
				walkingSpeed += 1;
			break;
		case MapTile::Bonus_Shield:
			shields++;
			break;
		case MapTile::Bonus_Slowness:
		case MapTile::Bonus_WetBombs:
		case MapTile::Bonus_Hyperactive:
			clearBadBonuses();
			bad_timer = 15000;
			bad_bonus = bon;
			break;
	}

	// fire a bomb everytime is possible if hyperactive
	if (bad_bonus == MapTile::Bonus_Hyperactive)
	{
		fire();
	}
}

void Character::clearBadBonuses()
{
	if (bad_bonus == MapTile::Bonus_Slowness)
	{
		bad_bonus = MapTile::Bonus_None;
		changeWalk(direction, currentSpeed > 0);
	}
	bad_bonus = MapTile::Bonus_None;
	bad_timer = -1;
}



bool Character::tryMoveTo(qreal& x, qreal& y)
{
	//TODO maybe should check entire line from
	// current pos to new. Currently is checking only destination
	// which is causing bad touching walls etc

	Map *map = field->getMap();

	if (x < 0.0 || y < 0.0 || x > map->getRealWidth() || y > map->getRealHeight())
		return false;


	// rect of new character position
	QRect new_rect(x,y,MapTile::SIDESIZE, MapTile::SIDESIZE);

	// bombs
	for(int i = 0; i < this->field->objectsCount(); i++)
	{
		GameObject *obj;
		obj = field->object(i);
		if (obj->type == GameObject::BOMB)
		{
			// when this character is "in" bomb, continue, because it can be only if, it is newly placed bomb
 			if (obj->objectRect().intersects(objectRect()))
				continue;

			// if character is going into other bomb
			if (obj->objectRect().intersects(new_rect))
			{
				if (canKick)
				{
					kickBomb(static_cast<Bomb *>(obj));
				}
				return false;
			}
		}
	}


	// zed primo pode mnou - kdyby prece
	MapTile *tile = map->getTile(x, y);
	if (tile != NULL && !tile->walkeable())
	{
		currentLife = 0;
		return false;
	}

	// steny zdi - hrac ma nejakou sirku

	// on the right
	tile = map->getTile(x+MapTile::SIDESIZE/2.0 - 0.1, y);
	if (tile != NULL && !tile->walkeable())
		x = positionX;
	// on the left
	tile = map->getTile(x-MapTile::SIDESIZE/2.0+0.1, y);
	if (tile != NULL && !tile->walkeable())
		x = positionX;
	// at bottom
	tile = map->getTile(x, y+MapTile::SIDESIZE/2.0 - 0.1);
	if (tile != NULL && !tile->walkeable())
		y = positionY;
	// at top
	tile = map->getTile(x, y-MapTile::SIDESIZE/2.0+0.1);
	if (tile != NULL && !tile->walkeable())
		y = positionY;

	
	// corner right bottom
	tile = map->getTile(x+MapTile::SIDESIZE/2.0 - 0.1, y+MapTile::SIDESIZE/2.0-0.1);
	if (tile != NULL && !tile->walkeable())
	{
		if (direction == NORTH || direction == SOUTH)
			y = positionY;
		else
			x = positionX;
	}
	// corner right top
	tile = map->getTile(x+MapTile::SIDESIZE/2.0 - 0.1, y-MapTile::SIDESIZE/2.0);
	if (tile != NULL && !tile->walkeable())
	{
		if (direction == NORTH || direction == SOUTH)
			y = positionY;
		else
			x = positionX;
	}
	// corner left bottom
	tile = map->getTile(x-MapTile::SIDESIZE/2.0, y+MapTile::SIDESIZE/2.0-0.1);
	if (tile != NULL && !tile->walkeable())
	{
		if (direction == NORTH || direction == SOUTH)
			y = positionY;
		else
			x = positionX;
	}
	// corner left top
	tile = map->getTile(x-MapTile::SIDESIZE/2.0 , y-MapTile::SIDESIZE/2.0);
	if (tile != NULL && !tile->walkeable())
	{
		if (direction == NORTH || direction == SOUTH)
			y = positionY;
		else
			x = positionX;
	}

	return true;
}
