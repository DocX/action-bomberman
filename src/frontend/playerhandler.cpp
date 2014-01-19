/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "playerhandler.h"
#include <qevent.h>


PlayerHandler::PlayerHandler(ABomberman::Character* player, QObject* parent): QObject(parent)
{
	if (player == NULL)
		throw new std::bad_exception();
	
	_player = player;
	
	key_fire = 
	key_turn_down = 
	key_turn_left = 
	key_turn_right = 
	key_turn_up = Qt::Key_unknown;
	
	_direction_stack = DirectionKeyStack();

	_name = "NoName";

	score = 0;

	pix_left = pix_right = NULL;
	acceptsKey = true;

	computer_player = false;
}


void PlayerHandler::setKeyDown(Qt::Key key)
{
	key_turn_down = key;
}

void PlayerHandler::setKeyUp(Qt::Key key)
{
	key_turn_up = key;
}

void PlayerHandler::setKeyLeft(Qt::Key key)
{
	key_turn_left = key;
}

void PlayerHandler::setKeyRight(Qt::Key key)
{
	key_turn_right = key;
}

void PlayerHandler::setKeyFire(Qt::Key key)
{
	key_fire = key;
}

Qt::Key PlayerHandler::keyDown()
{
	return key_turn_down;
}

Qt::Key PlayerHandler::keyFire()
{
return key_fire;
}
Qt::Key PlayerHandler::keyLeft()
{
return key_turn_left;
}
Qt::Key PlayerHandler::keyRight()
{
return key_turn_right;
}
Qt::Key PlayerHandler::keyUp()
{
return key_turn_up;
}


ABomberman::Character* PlayerHandler::player()
{
	return _player;
}

inline void PlayerHandler::_key_press(bool condition, GameObject::Direction dir)
{
	if (condition)
	{
		_player->changeWalk(dir,true);
		_direction_stack.push(dir);
	}
}

void PlayerHandler::keyPressed(QKeyEvent* event)
{
	if (!acceptsKey || event->isAutoRepeat() || player()->isDead())
	{
		return;
	}
	
	if (event->key() == key_fire)
	{
		player()->fire();
		return;
	}
	
	_key_press(event->key() == key_turn_left, GameObject::WEST);
	_key_press(event->key() == key_turn_right, GameObject::EAST);
	_key_press(event->key() == key_turn_up, GameObject::NORTH);
	_key_press(event->key() == key_turn_down, GameObject::SOUTH);
}

void PlayerHandler::keyReleased(QKeyEvent* event)
{
	if (!acceptsKey || event->isAutoRepeat())
		return;
	
	GameObject::Direction d;
	
	if (event->key() == key_turn_left)
		d = GameObject::WEST;
	else if (event->key() == key_turn_right)
		d = GameObject::EAST;
	else if (event->key() == key_turn_up)
		d = GameObject::NORTH;
	else if (event->key() == key_turn_down)
		d = GameObject::SOUTH;
	else
		return;
	
	_direction_stack.remove(d);
	if (_direction_stack.peek(d))
		_player->changeWalk(d,true);
	else
		_player->changeWalk(_player->direction, false);

}

QString PlayerHandler::name()
{
	return _name;
}

void PlayerHandler::setName ( QString name )
{
	if (name.isEmpty())
		return;
	_name = name;
}





// DIRECTION STACK

DirectionKeyStack::DirectionKeyStack()
{
	this->count = 0;
}
void DirectionKeyStack::push(GameObject::Direction dir)
{
	if (count == 4)
		return;
	this->stack[count++] = dir;
}
bool DirectionKeyStack::peek(GameObject::Direction& peekDir)
{
	if (count > 0)
	{
		peekDir = this->stack[count-1];
		return true;
	}
	else
		return false;
}
void DirectionKeyStack::remove(GameObject::Direction dir)
{
	int i;
	for(i = 0; i < count; i++)
	{
		if (stack[i] == dir)
		{
			count--;
			break;
		}
	}
	for (i; i < count; i++)
	{
		stack[i] = stack[i+1];
	}
}
