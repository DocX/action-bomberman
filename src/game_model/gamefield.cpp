/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "gamefield.h"
#include <qdebug.h>
#include <qthread.h>
#include <qcoreevent.h>
#include <qmetatype.h>
#include "gameobject.h"
#include "character.h"
#include "map.h"
#include "maptile.h"
#include "mappathfinder.h"


using namespace ABomberman;

const int ABomberman::GameField::LOOPPERIOD = 10;



ABomberman::GameField::GameField(ABomberman::Map* map): QObject(NULL),
	objects()
{
	this->loopTimer = new QTimer(this);
	loopTimer->setInterval(LOOPPERIOD);
	loopTimer->stop();
	
	connect(loopTimer, SIGNAL(timeout()), this, SLOT(update()) );
	
	this->map = map;

	last_object_id = 1;
	play_time_msec = 60000;

	_currentState = GameState_Unknown;
	_finder_instance = new MapPathFinder(map,this);
	_finder_fresh = false;
}


void GameField::update()
{
	//qDebug() << "updating field";
	elapsed_play_msecs += start_time.restart();
	
	if (remindingPlayTime() <= 0)
	{
		endGame();
		return;
	}
	
	GameObject *tmp_object;
	
	for(int i = 0; i < objects.length(); i++)
	{
		tmp_object = objects.at(i);
		if (tmp_object != NULL)
		{
			tmp_object->update();
		}
	}
	
	for(int i = 0; i < objects.length(); i++)
	{
		tmp_object = objects.at(i);
		if (tmp_object != NULL)
		{
			tmp_object->react();
		}
	}

	int life_players = 0;
	for(int i = 0; i < objects.length(); i++)
	{
		tmp_object = objects.at(i);
		if (tmp_object != NULL && tmp_object->type == GameObject::PLAYER && !static_cast<Character *>(tmp_object)->isDead())
		{
			life_players++;
		}
	}

	if (life_players <= 1)
	{
		endGame();
		return;
	}
	
	emit updated();

	_finder_fresh = false;
}

void GameField::endGame()
{
	loopTimer->stop();
	_currentState = GameState_GameOver;

	emit this->roundEnd();
}

void GameField::start()
{
	start_time.start();
	
	loopTimer->setInterval(LOOPPERIOD);
	loopTimer->start();

	elapsed_play_msecs = 0;
	_currentState = GameState_Playing;
}

void GameField::togglePause(bool paused)
{
	if (paused)
	{
		_currentState = GameState_Paused;
		loopTimer->stop();
	}
	else if (_currentState != GameState_GameOver)
	{
		loopTimer->start();
		start_time.start();
		_currentState = GameState_Playing;
	}
}

bool GameField::paused()
{
	return _currentState == GameState_Paused;
}


void GameField::setPlayTime(qint32 msec)
{
	if (msec < 0)
		return;
	play_time_msec = msec;
}

GameField::GameState GameField::currentGameState()
{
	return _currentState;
}

qint32 GameField::remindingPlayTime()
{
	
	return play_time_msec - elapsed_play_msecs;
}

qint32 GameField::elapsedPlayTime()
{
	return elapsed_play_msecs;
}



Map* GameField::getMap()
{
	return this->map;
}

quint32 GameField::insertObject(GameObject* object)
{
	// find if already inserted
	GameObject *tmp_object;
	for(int i = 0; i < objects.length(); i++)
	{
		tmp_object = objects.at(i);
		if (tmp_object == object)
		{
			// exists
			return 0;
		}
	}
	
	objects.append(object);

	return last_object_id++;
}

bool GameField::removeObject(GameObject* object)
{
	return objects.removeOne(object);
}


GameObject* GameField::object(int index)
{
	if (index < 0 || index >= objects.count())
		return NULL;

	return objects.at(index);
}


int GameField::objectsCount()
{
	return objects.count();
}


GameField::~GameField()
{
	delete map;
	delete this->loopTimer;
	delete _finder_instance;
}

MapPathFinder* GameField::getCurrentFinder()
{
	if (!_finder_fresh)
	{
		_finder_instance->InitializeGraph();
	}

	return _finder_instance;
}




