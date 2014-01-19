/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_MAPTILE_H
#define ABOMBERMAN_MAPTILE_H
#include <qglobal.h>
#include "map.h"

class QRectF;
class QPointF;

namespace ABomberman {

class Bomb;



/**
 * Class representing map tile. Defines its type and bonus.
 */
class MapTile
{
public:
	enum TileType
	{
		Tile_Ground,
		Tile_Wall,
		Tile_Brick,
	};

	enum ExplosionAbsorbtion
	{
		/// Tile do not absorb explosion. eg ground
		Absorbtion_None,
		/// Tile absorb explosion in self. eg bricks
		Absorbtion_Inside,
		/// Tile absorb explosion on the side - explosion did not get into tile. eg wall
		Absorbtion_Untouched
	};

	enum Bonus
	{
		// DONT FORGET TO CHANGE VALUES IN getBonus()
		
		/// Random bonus. Should not be used for getBonus()
		Bonus_Random = -1,
		/// Random from good bonuses
		Bonus_RandomGood = -2,
		/// Random from bad bonuses
		Bonus_RandomBad = -3,
		/// no bonus
		Bonus_None = 0,
		/// more bomb bonus
		Bonus_Bomb = 1,
		/// Increase bomb distance
		Bonus_MorePower = 2,
		/// Increase player speed
		Bonus_Speed = 3,
		/// Add ability to kick bombs
		Bonus_CanKick = 4,

		/// Add shield to avoid one explosion
		Bonus_Shield = 5,
/*		/// Add ability to throw bomb
		Bonus_CanThrow,*/

		Bonus_Slowness = 6,

		Bonus_Hyperactive = 7,

		Bonus_WetBombs = 8
	};

	/**
	 * Creates new map tile with type ground and no bonus. New tile coordination is 0,0
	 */
	MapTile();

	/**
	 * Creates new map tile on given coordination. Type is ground and have no bonus
	 */
	MapTile(const MapCoord& coord );

	/// Point size of tile side. It is used globaly in this game.
	static const qreal SIDESIZE;

	/// Returns true if objects can walk onto this tile
	bool walkeable();

	/// Returns true if this tile change type by explosion.
	bool destructive();

	/// Returns tile type
	TileType type();

	/// Sets tile type.
	MapTile* setType(TileType type);

	/// Returns tile's coordination on map
	MapCoord coords();

	/// Set left top corner coorfinates to referenced variables
	void tilePosition(qreal& x, qreal& y);

	/// Returns left top corner point of this tile (for game objects it is center of tile)
	QPointF tilePosition();

	/// Returns rectangle with size and coordinations of this tile
	QRectF tileRect();

	/// Returns bonus on this tile and keep it here
	Bonus haveBonus();

	/// Give bonus and remove from this tile
	Bonus getBonus();

	MapTile* setBonus(Bonus bonus);

	/// call when tile is under bomb explosion. Returns true if it absorbs explosion
	/// or false if explosion can continue to next tile.
	ExplosionAbsorbtion absorbExplosion();

	/// Only determine absorbtion type but not demage
	ExplosionAbsorbtion absorbExplosionFake();
	
private:
	MapCoord _my_coord;

	TileType _type;
    Bonus _bonus;
	
};

}

#endif // ABOMBERMAN_MAPTILE_H
