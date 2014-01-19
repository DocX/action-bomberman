/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "computercharecter.h"
#include "gamefield.h"
#include <QDebug>
#include <QRect>
#include <qlinkedlist.h>
#include "mappathfinder.h"

using namespace ABomberman;


ComputerCharacter::ComputerCharacter(qreal initX, qreal initY, GameField* parentField): Character(initX, initY, parentField)
{
	reset();
}

void ComputerCharacter::reset()
{
	latest_path = NULL;
	previous_next_node_index = previous_disallowed_node = -1;
	recompute_countdown = 0;
	
    ABomberman::Character::reset();
}

void ComputerCharacter::update()
{
	if (isDead())
		return;
	
	// First thing I should check is dangerous.
	// If I am in bomb explosion area, get out of it

	const int cols = this->field->getMap()->getCols();
	const MapCoord my_coord = this->getMapTile()->coords();
	const int my_index = my_coord.row * cols + my_coord.col;

	// recompute only when reached next node in path or path is not set
	if (--recompute_countdown <= 0 || previous_next_node_index < 0 || my_index == previous_next_node_index)
	{
		this->recomputeDirection();

		recompute_countdown = 5;
	}
	
	// update position ...
    ABomberman::Character::update();
}

void ComputerCharacter::recomputeDirection()
{
	const int tilesCount = this->field->getMap()->tilesCount();
	const int rows = this->field->getMap()->getRows();
	const int cols = this->field->getMap()->getCols();
	const MapCoord my_coord = this->getMapTile()->coords();
	const int my_index = my_coord.row * cols + my_coord.col;

	MapPathFinderGraph *path = searchMyPathes();

	delete latest_path;

	// Here we have distances to all possible tiles.
	// Select tile which is not exposed by bomb and have smallest distance
	int next_node_index = -1;
	QList<int> next_node_candidates = QList<int>();
	qreal best_score_distance_ratio = -1;
	
	for (int i = 0; i < path->nodes_count; i++)
	{
		MapPathFinderGraphNode *currentNode = &path->nodes[i];

		if (currentNode->exploding || currentNode->explosive ||
			currentNode->real_distance <= 0 || previous_disallowed_node == i
		)
			continue;

		// search the best ratio of score/distance
		qreal cur_ratio;
		if (path->nodes[my_index].explosive)
		{
			// get out of dangerous area in shortest posible way
			// ratio is greater as is distance to unexplosive tile smaller
			cur_ratio = /*currentNode->explosive ? 0 :*/ (tilesCount - currentNode->real_distance);
		}
		else
		{
			if (currentNode->path_is_dangerous)
				continue; // dont go  from safe to dangerous;
			cur_ratio = currentNode->score()/currentNode->real_distance;
		}

		if (best_score_distance_ratio < cur_ratio)
		{
			next_node_candidates.clear();
			next_node_candidates.append(currentNode->index);
			best_score_distance_ratio = cur_ratio;
		}
		else if (best_score_distance_ratio == cur_ratio)
		{
			next_node_candidates.append(currentNode->index);
		}
	}

	if (next_node_candidates.count() > 0)
	{
		if (previous_next_node_index >= 0 && next_node_candidates.contains(previous_destination_index))
		{
			next_node_index = previous_destination_index;
		}
		else
		{
			next_node_index = next_node_candidates[qrand() % next_node_candidates.count()];
		}
	}

	// place bomb?
	if (this->bad_bonus != MapTile::Bonus_Slowness &&
		(!path->nodes[my_index].explosive || path->nodes[my_index].explode_timeout > 750) && // dont place bomb into dangerous area
		previous_next_node_index == my_index)
	{
		previous_disallowed_node = -1;
		if (path->nodes[my_index].score_place_bomb > 0 && field->getCurrentFinder()->canPlaceBomb(MapCoord(my_index/cols, my_index%cols), power()))
		{
			fire();
		}
		else
		{
			// disallow this node - next iteration go elsewhere
			previous_disallowed_node = my_index;
		}
	}
	
	// determine where to go

	if (next_node_index != -1)
	{
		// save selected destination for next iteration
		previous_destination_index = next_node_index;

		// determine where to go to the next_node (using same var)
		while (path->nodes[next_node_index].path_previous_node_index != -1
			&& path->nodes[next_node_index].path_previous_node_index != my_index)
		{
			next_node_index = path->nodes[next_node_index].path_previous_node_index;
		}

		previous_next_node_index = next_node_index;

		// if next is to right
		if (next_node_index == my_index + 1)
		{
			changeWalk(EAST, true);
		}
		// left
		else if (next_node_index == my_index - 1)
		{
			changeWalk(WEST, true);
		}
		// down
		else if (next_node_index == my_index + cols)
		{
			changeWalk(SOUTH, true);
		}
		// up
		else if (next_node_index == my_index - cols)
		{
			changeWalk(NORTH, true);
		}
		else // keep position
		{
			changeWalk(direction, false);
		}
	}
	else
	{
		// no good path found


		//field->togglePause(true);

		// align onto tile
		qreal dx, dy;
		dx = positionX - getMapTile()->tilePosition().x();
		dy = positionY - getMapTile()->tilePosition().y();
		if (dx > 0.1)
		{
			// go to left
			changeWalk(WEST, true);
		}
		else if (dx < -0.1)
		{
			changeWalk(EAST, true);
		}
		else if (dy > 0.1)
		{
			// go to left
			changeWalk(NORTH, true);
		}
		else if (dy < -0.1)
		{
			changeWalk(SOUTH, true);
		}
		else
			changeWalk(direction, false);	
		
		previous_next_node_index = -1;
		previous_destination_index = -1;
	}


	latest_path = path;

}


MapPathFinderGraph *ComputerCharacter::searchMyPathes()
{
	const int my_index = field->getMap()->getTileIndex(getMapTile());
	
	MapPathFinderGraph *path = field->getCurrentFinder()->getGraph();
	path->nodes[my_index].distance = 1;

	// set individual scores
	for (int i = 0; i < path->nodes_count; i++)
	{
		// destructible brics
		// to left from i-tile
		for (int d = 1; d <= this->power(); d++)
		{
			MapTile *t = field->getMap()->getTile(i - d);
			if (t == NULL || t->type() == MapTile::Tile_Wall || path->nodes[i-d].explosive) // dont place more bomb that needed
				break;
			if (t->type() == MapTile::Tile_Brick)
			{
				path->nodes[i].score_place_bomb += 50;
				break;
			}
		}
		// to right from i-tile
		for (int d = 1; d <= this->power(); d++)
		{
			MapTile *t = field->getMap()->getTile(i + d);
			if (t == NULL || t->type() == MapTile::Tile_Wall || path->nodes[i+d].explosive)
				break;
			if (t->type() == MapTile::Tile_Brick)
			{
				path->nodes[i].score_place_bomb += 50;
				break;
			}
		}
		// up from i-tile
		for (int d = 1; d <= this->power(); d++)
		{
			MapTile *t = field->getMap()->getTile(i - d*field->getMap()->getCols());
			if (t == NULL || t->type() == MapTile::Tile_Wall || path->nodes[i - d*field->getMap()->getCols()].explosive)
				break;
			if (t->type() == MapTile::Tile_Brick)
			{
				path->nodes[i].score_place_bomb += 50;
				break;
			}
		}
		// down from i-tile
		for (int d = 1; d <= this->power(); d++)
		{
			MapTile *t = field->getMap()->getTile(i + d *field->getMap()->getCols());
			if (t == NULL || t->type() == MapTile::Tile_Wall || path->nodes[i + d*field->getMap()->getCols()].explosive)
				break;
			if (t->type() == MapTile::Tile_Brick)
			{
				path->nodes[i].score_place_bomb += 50;
				break;
			}
		}
	}

	// scores for other players and ability to go throught bomb in current tile
	for(int i = 0; i < field->objectsCount(); i++)
	{
		GameObject *object = field->object(i);
		int obj_map_index = field->getMap()->getTileIndex(object->getMapTile());
		
		if (object != this && object->type == PLAYER && !static_cast<Character *>(object)->isDead())
		{
			path->nodes[obj_map_index].score_place_bomb += 250;

			// tiles around

			// left
			for (int d = 1; d <= bombsPower; d++)
			{
				MapTile *tile = field->getMap()->getTile(obj_map_index-d);
				if (tile != NULL && tile->absorbExplosionFake() == MapTile::Absorbtion_None)
				{
					// score is greater than directly on object because here cannot go throught bomb
					path->nodes[obj_map_index-d].score_place_bomb += 400 - (d * 20);
				}
				else
					break;
			}
			// right
			for (int d = 1; d <= bombsPower; d++)
			{
				MapTile *tile = field->getMap()->getTile(obj_map_index+d);
				if (tile != NULL && tile->absorbExplosionFake() == MapTile::Absorbtion_None)
				{
					path->nodes[obj_map_index+d].score_place_bomb += 400 - (d*20);
				}
				else
					break;
			}
			// down
			for (int d = 1; d <= bombsPower; d++)
			{
				MapTile *tile = field->getMap()->getTile(obj_map_index+d*field->getMap()->getCols());
				if (tile != NULL && tile->absorbExplosionFake() == MapTile::Absorbtion_None)
				{
					path->nodes[obj_map_index+d*field->getMap()->getCols()].score_place_bomb += 400 - d*20;
				}
				else
					break;
			}
			// up
			for (int d = 1; d <= bombsPower; d++)
			{
				MapTile *tile = field->getMap()->getTile(obj_map_index-d*field->getMap()->getCols());
				if (tile != NULL && tile->absorbExplosionFake() == MapTile::Absorbtion_None)
				{
					path->nodes[obj_map_index-d*field->getMap()->getCols()].score_place_bomb += 400 - d*20;
				}
				else
					break;
			}

			
		}
		else if (object->type == BOMB && object->objectRect().intersects(this->objectRect()))
		{
			path->nodes[obj_map_index].is_accessible = true;
		}
	}

	field->getCurrentFinder()->searchPathes(path);
	return path;
}


void ComputerCharacter::react()
{
    ABomberman::Character::react();
}


