/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_GAMEFIELD_H
#define ABOMBERMAN_GAMEFIELD_H
#include <QObject>
#include <QTimer>
//#include "map.h"
//#include "gameobject.h"
//#include "character.h"
#include <QTime>


namespace ABomberman {

class MapPathFinder;


struct MapCoord;
class Map;
class Character;
class GameObject;
	
/**
 * GameField maintains main game logic
 * Includes game-loop implemented using timer, list of objects and map
 */
class GameField : public QObject
{
	Q_OBJECT
	
	static const int LOOPPERIOD;
	
public:
	/**
	 * Constructs new GameField instance with given map instance.
	 *
	 * @param map map instance used as arena for new gamefield
	 */
	GameField(ABomberman::Map* map);
    virtual ~GameField();

	/// Returns pointer to game's map
	Map *getMap();

	/**
	 * Returns finder with updated graph stub according to update cycle.
	 * This stub generated lazy by calling this method, bud each update
	 * is invalidated.
	 */
	MapPathFinder *getCurrentFinder();

	/// Starts the game. It activates game timer.
	void start();

	/// Toggle game pause
	void togglePause(bool paused);

	/// Returns if game is paused
	bool paused();

	/// Set time to end of round
	void setPlayTime(qint32 msec);

	/// mseconds to end of round
	qint32 remindingPlayTime();

	/// mseconds from start of round
	qint32 elapsedPlayTime();

	/// Inserts object and return game-unique ID
	quint32 insertObject(GameObject *object);

	/**
	 * removes game object from field.
	 * 
	 * @param object pointer to object
	 * @return true if object is present and removed, false else
	 * 
	 */ 
	bool removeObject(GameObject *object);

	/**
	 * Returns count of ovbjects
	 */
	int objectsCount();

	/**
	 * Return pointer to object with given index.
	 *
	 * @param index index of object
	 * @return pointer to object or NULL if index is not valid
	 */
	GameObject* object(int index);

	enum GameState
	{
		GameState_Unknown,

		/// game is running
		GameState_Playing,

		/// game is paused
		GameState_Paused,

		/// game is over (time elapsed or no life players)
		GameState_GameOver,
	};	

	GameState currentGameState();
	
private:
	Map *map;
	QList<GameObject *> objects;
	//QList<Character *> players;
	QTimer *loopTimer;

	quint32 last_object_id;

	qint32 play_time_msec;
    QTime start_time;
	int elapsed_play_msecs;

	void endGame();
	GameState _currentState;

	/// finder instance
	MapPathFinder *_finder_instance;

	/// if finder is initialized during current update. This is set to false every update.
	bool _finder_fresh;

public slots:
	void update();
	
private slots:

	
	
signals:
	void updated();
	void roundEnd();
		
};

}

#endif // ABOMBERMAN_GAMEFIELD_H
