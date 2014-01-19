/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef GAMEINFO_H
#define GAMEINFO_H
#include <QtGlobal>
#include <QObject>

class PlayerHandler;
template<class T >
class QList;
namespace ABomberman {
class Map;
class Character;
class GameField;
}

using namespace ABomberman;

class GameInfo : public QObject
{
	Q_OBJECT

	PlayerHandler* currentWinner;
	bool _match_over;
	
public:


	
	Map* map;
	QList<PlayerHandler* >* players;

	/// round time in miliseconds
	qint32 round_time;

	int wins_to_end;
	int rounds_played;

	int initialBombPower;
	int initialBombCount;
	qreal initialWalkingSpeed;
	
	GameInfo(Map* map);
	PlayerHandler* addPlayer(Character* character);

	void startGame(GameField* field);

	PlayerHandler* getRoundWinner();
	
	/// Determines player with maximum score and returns if it is equal to wins_to_end.
	bool isEnd();
public slots:
    void roundEnd();
};

#endif // GAMEINFO_H
