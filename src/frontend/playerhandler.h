/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef PLAYERHANDLER_H
#define PLAYERHANDLER_H

#include "../game_model/character.h"
#include <qwidget.h>

using namespace ABomberman;


class DirectionKeyStack
{
	GameObject::Direction stack[4];
	int top;
	
public:
	DirectionKeyStack();
	void push(GameObject::Direction dir);
	void remove(GameObject::Direction dir);
	bool peek(GameObject::Direction &peekDir);
	int count;
};

class PlayerHandler : public QObject
{
	Character *_player;
	QString _name;
	
	Qt::Key key_turn_left;
	Qt::Key key_turn_right;
	Qt::Key key_turn_up;
	Qt::Key key_turn_down;
	Qt::Key key_fire;
	
	DirectionKeyStack _direction_stack;
	
	void _key_press(bool condition, GameObject::Direction dir);
	
public:
    PlayerHandler(Character *player, QObject* parent = 0);
	
	void setKeyLeft(Qt::Key key);
	Qt::Key keyLeft();
	void setKeyRight(Qt::Key key);
	Qt::Key keyRight();
	void setKeyUp(Qt::Key key);
	Qt::Key keyUp();
	void setKeyDown(Qt::Key key);
	Qt::Key keyDown();
	void setKeyFire(Qt::Key key);
	Qt::Key keyFire();
	
	void keyPressed(QKeyEvent *);
	void keyReleased(QKeyEvent *);

	QString name();
	void setName(QString name);
	
	Character* player();

	QPixmap *pix_right;
	QPixmap *pix_left;

	/// If is set to false, keys are not handled (for AI)
	bool acceptsKey;

	QColor color;

	int score;

	bool computer_player;
    QPixmap pix_bomb;
};

#endif // PLAYERHANDLER_H
