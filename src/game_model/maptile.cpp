/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "maptile.h"
#include "map.h"
#include <qpoint.h>
#include <qrect.h>
#include <stdlib.h>
#include "bomb.h"
#include <qlinkedlist.h>

using namespace ABomberman;

const qreal MapTile::SIDESIZE = 100.0;

MapTile::MapTile()
{
	_my_coord = MapCoord(0,0);
	_type = Tile_Ground;
	_bonus = Bonus_None;
}

ABomberman::MapTile::MapTile(const MapCoord& coord)
{
	_my_coord = coord;
	_type = Tile_Ground;
	_bonus = Bonus_None;
}

bool ABomberman::MapTile::walkeable()
{
	return _type == Tile_Ground;
}

bool MapTile::destructive()
{
	return _type == Tile_Brick;
}

ABomberman::MapCoord ABomberman::MapTile::coords()
{
	return this->_my_coord;
}

QPointF MapTile::tilePosition()
{
	return QPointF(_my_coord.col * SIDESIZE, _my_coord.row * SIDESIZE);
}

void MapTile::tilePosition(qreal& x, qreal& y)
{
	QPointF p = tilePosition();
	x = p.x();
	y = p.y();
}

MapTile* ABomberman::MapTile::setType(MapTile::TileType type)
{
	_type = type;
	return this;
}

MapTile::TileType MapTile::type()
{
	return _type;
}

MapTile::ExplosionAbsorbtion MapTile::absorbExplosionFake()
{
	switch(_type)
	{
		case Tile_Brick:
			return Absorbtion_Inside;
			break;
		case Tile_Wall:
			return Absorbtion_Untouched;
		default:
			// if there is bonus, it absorbs explosion
			if (haveBonus() !=  Bonus_None)
			{
				return Absorbtion_Inside;
			}
			return Absorbtion_None;
	}
}


MapTile::ExplosionAbsorbtion MapTile::absorbExplosion()
{
	switch(_type)
	{
		case Tile_Brick:
			_type = Tile_Ground;
			return Absorbtion_Inside;
			break;
		case Tile_Wall:
			return Absorbtion_Untouched;
		default:
			// if there is bonus, it absorbs explosion
			if (haveBonus() !=  Bonus_None)
			{
				// remove bonus
				_bonus = Bonus_None;
				return Absorbtion_Inside;
			}
			return Absorbtion_None;
	}
}

QRectF MapTile::tileRect()
{
	return QRectF(tilePosition(), QSizeF(SIDESIZE,SIDESIZE));
}


MapTile* MapTile::setBonus(MapTile::Bonus bonus)
{
	_bonus = bonus;
	return this;
}

MapTile::Bonus MapTile::haveBonus()
{
	return _bonus;
}

MapTile::Bonus MapTile::getBonus()
{
/*	if (_type != Tile_Ground)
	{
		return Bonus_None;
	}
	*/
	Bonus ret;
	switch(_bonus)
	{
		case Bonus_Random:
			ret = (Bonus)(qrand() % 9);
			break;
		case Bonus_RandomBad:
			ret = (Bonus)(qrand() % 3 + 6);
			break;
		case Bonus_RandomGood:
			ret = (Bonus)(qrand() % 5 + 1);
			break;
		default:
			ret = _bonus;
			break;
	}
	_bonus = Bonus_None;
	return ret;
}
