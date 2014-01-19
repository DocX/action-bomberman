/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "bomb.h"
#include "gamefield.h"
#include "map.h"
#include "maptile.h"
#include "character.h"
#include <qrect.h>


using namespace ABomberman;

Bomb::Bomb(Character* player, MapCoord coord, GameField* parentField) :
	GameObject(parentField)
{
	qreal x,y;
	parentField->getMap()->getTile(coord.row,coord.col)->tilePosition(x,y);
	positionX = x;
	positionY = y;
	
	_state = State_Waiting;
	_power = 5;
	
	owner_player = player;
	
	setTimeout(-1);
	type = BOMB;

	dist_east = -1;
	dist_west = -1;
	dist_north = -1;
	dist_south = -1;

	kick_speed = 0;

	explodedObjects = new QList<int>();
}


void Bomb::update()
{
	// UPDATE POSITION
	if (kick_speed > 0)
	{
		qreal newX = 0,newY = 0;

		QPoint p = directionMultipliers();
		newX = kick_speed * p.x() + positionX;
		newY = kick_speed * p.y() + positionY;

		if (tryMoveTo(newX, newY))
		{
			positionX = newX;
			positionY = newY;
		}
		else
		{
			// if is not posible to go to new position, stop moving
			kick_speed = 0;
		}
	}

	// countdown timer
	if (_state == State_Active)
	{
		if (countdown)
		{
			_timeout -= elapsedFromLastUpdate();
			if (_timeout < 0)
			{
				countdown = false;
				detonate();
				return; // Skip Exploding stage below
			}
		}
	}

	// propagate explosion and countdown explosion timer
	if (_state == State_Exploding)
	{

		_explosion_timeout -= elapsedFromLastUpdate();
		if (_explosion_timeout < 0)
		{
			_state = State_Done;
			remove();
			// this instance is now referenced by zero pointers
			// delete

			delete this;
			return;
		}
	}
	
    GameObject::update();
}

void Bomb::react()
{
    // if is exploding, propagate explosion to all objects newly appeared in explosion
    if (_state == State_Exploding)
	{
		propagateExplosion();
	}

	 //MapTile::Bonus bon = getMapTile()->getBonus();
}

bool Bomb::isInExplosion(GameObject* object)
{
	return isInExplosion(object->objectRect());
}


bool Bomb::isInExplosion(MapCoord object_coord)
{
	return isInExplosion(
		QRectF(object_coord.col*MapTile::SIDESIZE, object_coord.row*MapTile::SIDESIZE, MapTile::SIDESIZE, MapTile::SIDESIZE)
		);
	

	// previous version checks center of objects (their map coordinations)
/*	return
		(object_coord.col >= bomb_coord.col-dist_west && object_coord.col <= bomb_coord.col+dist_east && object_coord.row == bomb_coord.row) ||
		(object_coord.row >= bomb_coord.row-dist_north && object_coord.row <= bomb_coord.row+dist_south && object_coord.col == bomb_coord.col);
	*/	
}


bool Bomb::isInExplosion(QRectF object_rect)
{
	computeDistances();

	MapCoord bomb_coord = this->getMapTile()->coords();

	QRectF horizontal_explosion(
		(bomb_coord.col-dist_west) * MapTile::SIDESIZE + MapTile::SIDESIZE/4.0,
		bomb_coord.row * MapTile::SIDESIZE + MapTile::SIDESIZE/4.0,
		(dist_west+1+dist_east) * MapTile::SIDESIZE - MapTile::SIDESIZE/2.0,
		MapTile::SIDESIZE/2.0);
	QRectF vertical_explosion(
		bomb_coord.col * MapTile::SIDESIZE + MapTile::SIDESIZE/4.0,
		(bomb_coord.row-dist_north) * MapTile::SIDESIZE + MapTile::SIDESIZE/4.0,
		MapTile::SIDESIZE/2.0,
		(dist_south+1+dist_north) * MapTile::SIDESIZE - MapTile::SIDESIZE/2.0);

	return vertical_explosion.intersects(object_rect) || horizontal_explosion.intersects(object_rect);
}


void Bomb::propagateExplosion(bool first)
{
	
	// search throught object for some that is in explosion
	for	(int i = 0; i < field->objectsCount(); i++)
	{
		GameObject *object = field->object(i);
		if (object == this)
			continue;
		if (!explodedObjects->contains(object->objectID()) && isInExplosion(object))
		{
			object->handleMessage(this,Message_in_explosion, first);
			explodedObjects->append(object->objectID());
		}
	}
}


void ABomberman::Bomb::setTimeout(int msec)
{
	if (msec > 0);
	
	_timeout = msec;
}

void Bomb::setCountdownActive(bool active)
{
	countdown = active;
}

inline void _tile_explode(MapTile *tile, int& cur_dist, int& final_dist)
{
	if (tile != NULL)
	{
		switch(tile->absorbExplosionFake())
		{
			case MapTile::Absorbtion_None:
				break;
			case MapTile::Absorbtion_Inside:
				final_dist = cur_dist;
				break;
			case MapTile::Absorbtion_Untouched:
				final_dist = cur_dist - 1;
				break;
		}
	}
	else
	{
		// is outside arena
		final_dist = cur_dist - 1;
	}
}

void Bomb::computeDistances()
{
	if (state() == State_Exploding)
		return;
	
	MapCoord bomb_coord = this->getMapTile()->coords();
	Map *map = this->field->getMap();

	dist_east = dist_west = dist_north = dist_south = -1;
	
	for (int dist = 1; dist <= _power; dist++)
	{
		// east
		if (dist_east < 0)
		{
			_tile_explode(
				map->getTile(bomb_coord.row,bomb_coord.col + dist),
				dist,
				dist_east
			);
		}
		// west
		if (dist_west < 0)
		{
			_tile_explode(
				map->getTile(bomb_coord.row,bomb_coord.col - dist),
				dist,
				dist_west
			);
		}
		// south
		if (dist_south < 0)
		{
			_tile_explode(
				map->getTile(bomb_coord.row + dist,bomb_coord.col),
				dist,
				dist_south
			);
		}
		// north
		if (dist_north < 0)
		{
			_tile_explode(
				map->getTile(bomb_coord.row - dist,bomb_coord.col),
				dist,
				dist_north
			);
		}
	}
	if (dist_east < 0)
		dist_east = _power;
	if (dist_west < 0)
		dist_west = _power;
	if (dist_south < 0)
		dist_south = _power;
	if (dist_north < 0)
		dist_north = _power;
}


void ABomberman::Bomb::detonate()
{
	if (_state == State_Exploding)
		return;

	// -- destroy walls --
	Map *map = this->field->getMap();
	MapCoord bomb_coord = this->getMapTile()->coords();

	computeDistances();
	for (int dist = 1; dist <= _power; dist++)
	{
		// east
		if (dist_east >= dist)
		{
			map->getTile(bomb_coord.row,bomb_coord.col + dist)->absorbExplosion();
		}
		// west
		if (dist_west >= dist)
		{
			map->getTile(bomb_coord.row,bomb_coord.col - dist)->absorbExplosion();
		}		
		// south
		if (dist_south >= dist)
		{
			map->getTile(bomb_coord.row + dist,bomb_coord.col)->absorbExplosion();
		}
		// north
		if (dist_north >= dist)
		{
			map->getTile(bomb_coord.row - dist,bomb_coord.col)->absorbExplosion();
		}		
	}


	// order to map grid
	positionX = bomb_coord.col * MapTile::SIDESIZE;
	positionY = bomb_coord.row * MapTile::SIDESIZE;
	kick_speed = 0;
	
	
	// -- propagate explosion to other objects --
	_state = State_Exploding;
	_explosion_timeout = 300;
	propagateExplosion(true);

	// this removes bomb from player's stack
	owner_player->handleMessage(this, Message_bomb_detonated);
}

bool Bomb::countdownActive()
{
	return countdown;
}

int ABomberman::Bomb::power()
{
	return _power;
}

void ABomberman::Bomb::setPower(int power)
{
	if (_state == State_Exploding)
		return;
	
	this->_power = power;
	computeDistances();
}

int ABomberman::Bomb::timeout()
{
	return _timeout;
}

void Bomb::setState(Bomb::BombState state)
{
	_state = state;
}

Bomb::BombState Bomb::state()
{
	return _state;
}

void Bomb::handleMessage(ABomberman::GameObject* sender, GameObject::MessageType type, bool first)
{
    switch(type)
	{
		case Message_in_explosion:
			if (first)
			{
				//_timeout = 75;
				//setCountdownActive(true);
				detonate();
			}
			break;
	}
		
}



bool Bomb::tryMoveTo(qreal& x, qreal& y)
{
	//TODO maybe should check whole line from current pos to new. Currently is checking only destination.

	Map *map = field->getMap();

	if (x < 0.0 || y < 0.0 || x > map->getRealWidth() || y > map->getRealHeight())
		return false;

	// at first order into map row or column
	if (direction == NORTH || direction == SOUTH)
	{
		x = getMapTile()->tileRect().x();
	}
	else
	{
		y = getMapTile()->tileRect().y();
	}

	// rect of new character position
	QRect new_rect(x,y,MapTile::SIDESIZE, MapTile::SIDESIZE);

	// bomby a hraci
	for(int i = 0; i < this->field->objectsCount(); i++)
	{
		GameObject *obj;
		obj = field->object(i);

		if (obj == this)
			continue;
		
		if (obj->type == GameObject::BOMB)
		{
			
			// if bomb is going into other bomb
			if (obj->objectRect().intersects(new_rect))
			{
				return false;
			}
		}
		else if (obj->type == PLAYER)
		{
			// bomb is touching some player
			if (obj->objectRect().intersects(new_rect))
			{
				return false;
			}			
		}
	}

	// zed primo pode mnou
	MapTile *tile = map->getTile(x, y);
	if (tile != NULL && !tile->walkeable())
		return false;

	// steny zdi - hrac ma nejakou sirku
	tile = map->getTile(x+MapTile::SIDESIZE/2.0 - 0.1, y);
	if (tile != NULL && !tile->walkeable())
		return false;
	tile = map->getTile(x-MapTile::SIDESIZE/2.0, y);
	if (tile != NULL && !tile->walkeable())
		return false;
	tile = map->getTile(x, y+MapTile::SIDESIZE/2.0 - 0.1);
	if (tile != NULL && !tile->walkeable())
		return false;
	tile = map->getTile(x, y-MapTile::SIDESIZE/2.0);
	if (tile != NULL && !tile->walkeable())
		return false;

	// rohy
	tile = map->getTile(x+MapTile::SIDESIZE/2.0 - 0.1, y+MapTile::SIDESIZE/2.0-0.1);
	if (tile != NULL && !tile->walkeable())
		return false;
	tile = map->getTile(x+MapTile::SIDESIZE/2.0 - 0.1, y-MapTile::SIDESIZE/2.0);
	if (tile != NULL && !tile->walkeable())
		return false;
	tile = map->getTile(x-MapTile::SIDESIZE/2.0, y+MapTile::SIDESIZE/2.0-0.1);
	if (tile != NULL && !tile->walkeable())
		return false;
	tile = map->getTile(x-MapTile::SIDESIZE/2.0 , y-MapTile::SIDESIZE/2.0);
	if (tile != NULL && !tile->walkeable())
		return false;

	return true;
}

void ABomberman::Bomb::kick(GameObject::Direction dir, int speed)
{
	kick_speed = speed;
	direction = dir;

	qreal newX = 0,newY = 0;

	QPoint p = directionMultipliers();
	// move immediately after kick - make space betweek kicker and bomb - if it is possible
	newX = kick_speed * p.x()*5 + positionX;
	newY = kick_speed * p.y()*5 + positionY;

	if (tryMoveTo(newX, newY))
	{
		positionX = newX;
		positionY = newY;
	}
}

int ABomberman::Bomb::ownerObjectId()
{
	return owner_player->objectID();
}
