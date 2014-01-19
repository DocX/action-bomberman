/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "gameinfo.h"
#include <qlist.h>
#include "playerhandler.h"
#include "../game_model/gamefield.h"

GameInfo::GameInfo(Map* map)
{
	this->map = map;
	this->players = new QList<PlayerHandler* >();

	this->round_time = 60;
	this->rounds_played = 0;
	this->wins_to_end = 3;

	initialBombCount = 1;
	initialBombPower = 2;
	initialWalkingSpeed = 5;

	_match_over = false;
}


PlayerHandler* GameInfo::addPlayer(Character* character)
{
	PlayerHandler* h = new PlayerHandler(character);
	players->append(h);
	return h;
}

void GameInfo::startGame(GameField* field)
{
	// add players
	int p = 0;
	foreach(PlayerHandler* h, *players)
	{
		MapCoord c = map->getStartingPosition(p++);
		h->player()->setPosition(c);
		h->player()->reset();
		h->player()->setBombCount(initialBombCount);
		h->player()->setBombPower(initialBombPower);
		h->player()->setWalkingSpeed(initialWalkingSpeed);
		h->player()->addToField(field);
	}

	// set play time
	field->setPlayTime(round_time);

	connect(field, SIGNAL(roundEnd()), this, SLOT(roundEnd()));
}

void GameInfo::roundEnd()
{
	rounds_played++;
	
	PlayerHandler *winner = NULL;
	int life_num = 0, max_score = 0;

	foreach(PlayerHandler *p, *this->players)
	{
		if (!p->player()->isDead())
		{
			life_num++;
			winner = p;
		}
		if (p->score > max_score)
			max_score = p->score;
	}

	if (life_num == 1)
	{
		winner->score++;
		if (winner->score > max_score)
			max_score = winner->score;
		currentWinner = winner;
	}
	else
	{
		currentWinner = NULL;
	}

	if (max_score >= wins_to_end)
	{
		_match_over = true;
	}
}

bool GameInfo::isEnd()
{
	return _match_over;
}

PlayerHandler* GameInfo::getRoundWinner()
{
	return currentWinner;
}


