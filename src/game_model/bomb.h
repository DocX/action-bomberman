/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_BOMB_H
#define ABOMBERMAN_BOMB_H

#include "gameobject.h"
#include <QRect>


namespace ABomberman {

class Character;

/**
 * Represents bomb object. It countsdown bomb timer,
 * compute explosion distances and contacts other objects on
 * game field about explosion.
 *
 */
class Bomb : public GameObject
{
public:
	enum BombState {
		/// Bomb is not activated but is present in game.
		State_Waiting,
		
		/// Activated and excepted to detonate
		State_Active,
		
		/// Detonated and explosion is present. Can harm players.
		State_Exploding,
		
		/// Exploded or deactivated and is going to remove itself.
		State_Done
	};
	
private:
	/// if countdown is active, number of msec to detonate
	int _timeout;
	
	/// countdown to detonation enabled
	bool countdown;
	
	/// explosion number of tiles. -1 whole map.
	int _power;
	
	/// msec to change from State_Exploding to State_Done
	int _explosion_timeout;
	
	BombState _state;

	/// moving_speed due to kick
	int kick_speed;
	
	Character *owner_player;

	/// List of objectIDs which was informed about explostion
	QList<int> *explodedObjects;

	void propagateExplosion(bool first = false);


	
public:
	/**
	 * Constructs Bomb instance placed on given map coordination and with
	 * pointer to given owner.
	 */
    Bomb(ABomberman::Character* player, ABomberman::MapCoord coord, ABomberman::GameField* parentField);

	/**
	 * Updates time to explode and when time is zero, call propagateExplosion
	 */
    virtual void update();
	
	/**
	 * Propagate explosion to newly appeared objects
	 */
    virtual void react();

	/**
	 * Handles message from other objects. Reacts to in_explosion message
	 * by detonating itself
	 */
    virtual void handleMessage(ABomberman::GameObject* sender, ABomberman::GameObject::MessageType type, bool first);

	/**
	 * Determine if given rectangle is in explosion. Rectangle is in explosion
	 * if intersects line of half of TileSide width in center of exploding tiles
	 */
	bool isInExplosion(QRectF object_rect);

	/// Determine if given game object is in explosion or tile in explosion range.
	bool isInExplosion(GameObject *object);
	
	/// Determine if given tile coordination is in explosion or in explosion range.
	bool isInExplosion(MapCoord );

	/// Sets number of miliseconds to detonate bomb
	void setTimeout(int msec);
	
	/// Number of miliseconds to detonate bomb
	int timeout();

	/// Set countdown to detonation
	void setCountdownActive(bool active);

	/// Returns if countdown to detonation is active
	bool countdownActive();

	/// Set power of bomb
	void setPower(int power);

	/// Returns power of bomb
	int power();
	
	void setState(BombState state);
	BombState state();

	/**
	 * Detonates bomb. Explosion will be propagated to each affected object
	 * and will be changed map
	 */
	void detonate();

	/**
	 * Precompute and saves distances of explosion range in each direction.
	 * during exploding state is not effective, due to preserve explosion state.
	 */
	void computeDistances();

	/// explosion distance to east.
    int dist_east;

	/// explosion distance to west. 
    int dist_west;

	/// explosion distance to north. 
    int dist_north;

	/// explosion distance to south. 
    int dist_south;

	/**
	 * Kicks bomb in given direction and speed. It actually
	 * sets objects speed and direction
	 */
	void kick(Direction dir, int speed);

	/**
	 * Returns object id of owner player
	 */
	int ownerObjectId();
protected:

	/**
	 * Determine if new coordinations are possible to set and
	 * changes it by current situation.
	 * 
	 * @returns True if new coordinations are possible, false else
	 */
	bool tryMoveTo(qreal& x, qreal& y);
	
};

}

#endif // ABOMBERMAN_BOMB_H
