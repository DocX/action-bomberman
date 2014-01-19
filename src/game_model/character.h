/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_CHARACTER_H
#define ABOMBERMAN_CHARACTER_H
#include "gameobject.h"
#include "bomb.h"
#include "maptile.h"

template<class T >
class QLinkedList;
namespace ABomberman {

/**
 * Class representing an player of game. It handles
 * moving, interacting with map and other objects
 * and placing bombs
 */
class Character : public GameObject
{
protected:
	
	/// Current player health 
	int currentLife;
	
	/// Number of shields
	int shields;
	
	/// Number of bombs, that player have
	int bombsCount;

	/// Bomb power in number of tiles of explosion for newly placed bombs.
    int bombsPower;
	
	/// Property of base speed of player. It is copied to currentSpeed when player is moving.
	qreal walkingSpeed;

	/// Current speed of player
	qreal currentSpeed;
	
	/// Pointers to player's bombs (for remote control)
	QLinkedList<Bomb *> *myBombs;

	/// If have bonus for kicking bombs
	bool canKick;

	/// If got bad bonus, number of msecs to end of its effect
	int bad_timer;

	/// Type of bad bonus got. Bonus_none if no bad bonus got.
	ABomberman::MapTile::Bonus bad_bonus;

	/// Removes bad bonus effect
	void clearBadBonuses();

	/**
	 * Determine if given new coordinates are posible to go.
	 * If yes, their are changed by map situation.
	 *
	 * @returns true if given position is posible to go, false else
	 */
	bool tryMoveTo(qreal& x, qreal& y);
	
public:
	/**
	 * Constructs instance of Character on given position
	 *
	 * @param initX X coordinate of character position
	 * @param initY Y coordinate of character position
	 */
    Character(qreal initX, qreal initY, GameField* parentField);

	/**
	 * Changes walk direction and state if player is moving or not.
	 *
	 * @param direction direction of new walks
	 * @param state true to start moving, false to stop moving
	 */
	void changeWalk(Direction direction, bool state);

	/// Place a bomb
	void fire();

	/**
	 * Implements virtual update method. It updates position by
	 * current speed and direction and if bad bonus, counts down its
	 * timer.
	 * Effective only when player is not dead.
	 */
    virtual void update();

	/**
	 * Implements virtual react method. Checks bonus on current map tile
	 * and get it if some is present.
	 */
    virtual void react();

	/**
	 * Handles messages from other objects. Its effective only when player is alive.
	 * React to in_explosion by setting state to dead
	 * React to bomb_detonated by removing owned bomb from bombs list
	 */
    virtual void handleMessage(ABomberman::GameObject* sender, ABomberman::GameObject::MessageType type, bool first = false);

	/// Life number (Only 100 and 0 yet)
	int life();
	
	/**
	 * Kicks given bomb by current direction, if is possible
	 *
	 * @param bomb pointer to bomb intended to kick
	 */
	void kickBomb(ABomberman::Bomb* bomb);
	
	/// If player is dead
	bool isDead();
	
	/// Returns player's count of bombs
	int bombs();
	/// Returns player's bomb power (number of tiles of explosion)
	int power();
	/// Returns number of shields.
	int shieldsCount();

	/// Returns count of msecond reminding to clear bad bonus. If less than zero, no bad bonus is effective.
	int badBonusReminding();

	/// Reset player internal values
	virtual void reset();
	
    void setWalkingSpeed(qreal arg1);
    void setBombPower(int arg1);
    void setBombCount(int arg1);

	bool isWalking();
	
};

}

#endif // ABOMBERMAN_CHARACTER_H
