/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_GAMEOBJECT_H
#define ABOMBERMAN_GAMEOBJECT_H

#include <qglobal.h>
//#include "map.h"
#include <QDate>

class QPoint;
class QRectF;

namespace ABomberman {

class MapTile;
class GameField;
struct MapCoord;

/**
 * Class representing all moving object in game.
 * It is abstract, so each type of object must extends
 * this class and implement own methods. Methods update
 * and react are called by field in intervals.
 */
class GameObject
{
	/// internal game-field unique object ID
	quint32 object_id;

protected:
	/**
	 * Constructs GameObject instance and adds itself into field.
	 * 
	 * @param parentField NULL if object is not intended yet to be part of some gamefield
	 * or pointer to instance of GameField into which to add. 
	 */
	GameObject(ABomberman::GameField* parentField);

	
public:
	/**
	 * Adds itself into given field. It is used when object is created before field
	 * and now is field created.
	 *
	 * @param parentField poitner to instance of game field into which add itself. If
	 * NULL given, nothing is done.
	 */
	void addToField(GameField* parentField);

	// X coordinate of real position on map
	qreal positionX;
	// Y coordinate of real position on map
	qreal positionY;

	/// List of available directions
	enum Direction {
		NORTH = 0,
		EAST = 1,
		SOUTH = 2,
		WEST = 3
	};

	/// List of available object types
	enum ObjectType {
		UNSPECIFIED = 0,
		PLAYER,
		BOMB,
	};

	/// List of messages available to send to objects by handleMessage()
	enum MessageType {
		Message_unknown,
		
		/// tels object that it is in explosion
		Message_in_explosion,

		/// should send to player when its bomb detonated
		Message_bomb_detonated
	};

	/// Object's type
	ObjectType type;

	/// Current direction of objects
	Direction direction;

	/**
	 * Updates lastUpdateTime of current object. This method have to be
	 * overrided by extended class. Should be called from overriden methods.
	 */
	virtual void update();

	/**
	 * Called by field after all objects updated.
	 */
	virtual void react();

	/**
	 * React to messages sent by other objects
	 * @param sender pointer to instance of sender object
	 * @param type what message is sent
	 * @param first additional information if this message is sent by first to current object
	 */
	virtual void handleMessage(GameObject* sender, MessageType type, bool first = false);

	/// Returns pointer to Tile instance above which is object located on the field's map.
	MapTile *getMapTile();

	/// Returns rectangle in real coordinates of current object
	QRectF objectRect();

	/// Unique object id. This is set by field.
	quint32 objectID();

	/// Sets position to center of given tile
	void setPosition(MapCoord pos);
protected:

	/// Pointer to game field, where object lives
	GameField *field;

	/// remove itself from game field
	void remove();

	/// timestamp of time when was last update() called
	qint32 lastUpdateFieldTime;

	/**
	 * Returns point with zero or one in coresponding X or Y coordinate
	 * coresponding to current direction.
	 * For example, if direction is NORTH, returns point (0, -1)
	 *
	 * @return point with +/- 1 or 0 in X or Y coresponding to current direction
	 */
	QPoint directionMultipliers();

	/// Returns msecs elapsed from last update called.
	qint32 elapsedFromLastUpdate();
};

}

#endif // ABOMBERMAN_GAMEOBJECT_H
