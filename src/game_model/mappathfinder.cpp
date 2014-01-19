/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "mappathfinder.h"
#include "gameobject.h"
#include "bomb.h"
#include "maptile.h"
#include <qlinkedlist.h>
#include "gamefield.h"
#include <QStack>
#include <math.h>

using namespace ABomberman;


MapPathFinderGraphNode::MapPathFinderGraphNode()
{
	exploding = explosive = false;
	path_is_dangerous = false;
	distance = -1;
	real_distance = 0;
	path_previous_node_index = -1;
	score_to_go = score_place_bomb = 0;
	is_accessible = true;
	explode_timeout = 100000;

}

MapPathFinderGraphNode::MapPathFinderGraphNode(MapPathFinderGraphNode* node)
{
	distance = -1;
	real_distance = 0;
	explode_timeout = node->explode_timeout;
	path_previous_node_index = -1;
	exploding = node->exploding;
	explosive = node->explosive;
	is_accessible = node->is_accessible;
	score_to_go = node->score_to_go;
	score_place_bomb = node->score_place_bomb;
	index = node->index;
	path_is_dangerous = false;
}


void MapPathFinderGraphNode::relaxDistance (MapPathFinderGraphNode *from)
{
	if (from == NULL)
		return;

	qreal cost_to = 5;

	// path to dangerous area has greater cost
/*	if (explosive)
	{
		cost_to = 0.5;
	}
	else
	{
		cost_to = 0.1;
	}
*/
	// higher score decreases cost and visa versa
	// score can be negative, so use it in exponential function
	
	cost_to = cost_to / pow(1.01, score());

	if (this->explode_timeout < from->explode_timeout)
		cost_to += 10;
	
	if (distance < 0 || distance > from->distance + cost_to)
	{
		distance = from->distance + cost_to;
		path_previous_node_index = from->index;
		real_distance = from->real_distance + 1;
		path_is_dangerous = from->explosive || this->explosive || from->path_is_dangerous;
	}
}


bool MapPathFinderGraphNode::existsEdgeTo(MapPathFinderGraphNode* to)
{
	return
		to->is_accessible &&
		!to->exploding;// &&
		//!(!this->explosive && to->explosive);
}

void MapPathFinderGraphNode::relaxDistanceIfExistsEdge(MapPathFinderGraphNode* from)
{
	if (from->existsEdgeTo(this))
	{
		relaxDistance(from);
	}
}

int MapPathFinderGraphNode::score()
{
	return score_place_bomb + score_to_go;
}




MapPathFinderGraph::MapPathFinderGraph(int nodes)
{
	nodes_count = nodes;
	this->nodes = new MapPathFinderGraphNode[nodes];

	for (int i = 0; i < nodes_count; i++)
	{
		this->nodes[i].index = i;
	}
}

MapPathFinderGraph::MapPathFinderGraph(MapPathFinderGraph* graph)
{
	nodes_count = graph->nodes_count;
	
	this->nodes = new MapPathFinderGraphNode[nodes_count];

	for(int i = 0; i <nodes_count; i++)
	{
		nodes[i] = MapPathFinderGraphNode(&graph->nodes[i]);
	}
}
	




ABomberman::MapPathFinder::MapPathFinder(Map* map, GameField* field)
{
	_map = map;
	_field = field;
	_graph_stub = NULL;
}

void MapPathFinder::InitializeGraph()
{
	const int tilesCount = _field->getMap()->tilesCount();
	const int rows = _field->getMap()->getRows();
	const int cols = _field->getMap()->getCols();

	_graph_stub = new MapPathFinderGraph(tilesCount);

	// tag all tiles in explosive area or in explosion
	for(int i = 0; i < _field->objectsCount(); i++)
	{
		GameObject *obj = _field->object(i);
		MapCoord obj_coord = obj->getMapTile()->coords();
		int obj_index = obj_coord.col + obj_coord.row * cols;
		
		if (obj->type == GameObject::BOMB)
		{
			Bomb *bomb = static_cast<Bomb *>(obj);
			bool exploding = (bomb->state() == Bomb::State_Exploding);
			bool go_there = !exploding && !(bomb->countdownActive() && bomb->timeout() < 200);
			int score_penalty = (bomb->countdownActive() && bomb->timeout() > 0) ? (750000 / bomb->timeout()) : 250;
			
			// generaly cannot go throught bombs
			_graph_stub->nodes[obj_index].is_accessible = false;
			_graph_stub->nodes[obj_index].explosive = true;
			_graph_stub->nodes[obj_index].exploding = exploding;
			_graph_stub->nodes[obj_index].score_to_go -= score_penalty;
			_graph_stub->nodes[obj_index].explode_timeout = bomb->timeout() < _graph_stub->nodes[obj_index].explode_timeout ? bomb->timeout() : _graph_stub->nodes[obj_index].explode_timeout;


			// tag all nodes in explosive range
			static_cast<Bomb *>(obj)->computeDistances();
			for (int d = 1; d <= static_cast<Bomb *>(obj)->dist_west; d++)
			{
				_graph_stub->nodes[obj_index - d].is_accessible = go_there;
				_graph_stub->nodes[obj_index - d].explosive = true;
				_graph_stub->nodes[obj_index - d].exploding = exploding;
				_graph_stub->nodes[obj_index - d].score_to_go -= score_penalty;
				_graph_stub->nodes[obj_index - d].explode_timeout = bomb->timeout() < _graph_stub->nodes[obj_index - d].explode_timeout ? bomb->timeout() : _graph_stub->nodes[obj_index - d].explode_timeout;
			}
			for (int d = 1; d <= static_cast<Bomb *>(obj)->dist_east; d++)
			{
				_graph_stub->nodes[obj_index + d].is_accessible = go_there;
				_graph_stub->nodes[obj_index + d].explosive = true;
				_graph_stub->nodes[obj_index + d].exploding = exploding;
				_graph_stub->nodes[obj_index + d].score_to_go -= score_penalty;
				_graph_stub->nodes[obj_index + d].explode_timeout = bomb->timeout() < _graph_stub->nodes[obj_index+d].explode_timeout ? bomb->timeout() : _graph_stub->nodes[obj_index+d].explode_timeout;
			}
			for (int d = 1; d <= static_cast<Bomb *>(obj)->dist_north; d++)
			{
				_graph_stub->nodes[obj_index - d*cols].is_accessible = go_there;
				_graph_stub->nodes[obj_index - d*cols].explosive = true;
				_graph_stub->nodes[obj_index - d*cols].exploding = exploding;
				_graph_stub->nodes[obj_index - d*cols].score_to_go -= score_penalty;
				_graph_stub->nodes[obj_index - d*cols].explode_timeout = bomb->timeout() < _graph_stub->nodes[obj_index-d*cols].explode_timeout ? bomb->timeout() : _graph_stub->nodes[obj_index-d*cols].explode_timeout;
			}
			for (int d = 1; d <= static_cast<Bomb *>(obj)->dist_south; d++)
			{
				_graph_stub->nodes[obj_index + d*cols].is_accessible = go_there;
				_graph_stub->nodes[obj_index + d*cols].explosive = true;
				_graph_stub->nodes[obj_index + d*cols].exploding = exploding;
				_graph_stub->nodes[obj_index + d*cols].score_to_go -= score_penalty;
				_graph_stub->nodes[obj_index + d*cols].explode_timeout = bomb->timeout() < _graph_stub->nodes[obj_index+d*cols].explode_timeout ? bomb->timeout() : _graph_stub->nodes[obj_index+d*cols].explode_timeout;
			}

		}
	}

	// initialize edges and scores
	for(int i = 0; i < tilesCount; i++)
	{
		// scores
		MapTile::Bonus bon = _field->getMap()->getTile(i)->haveBonus();
		switch (bon)
		{
			case MapTile::Bonus_CanKick:
			case MapTile::Bonus_MorePower:
			case MapTile::Bonus_Bomb:
			case MapTile::Bonus_RandomGood:
			case MapTile::Bonus_Shield:
			case MapTile::Bonus_Speed:
				_graph_stub->nodes[i].score_to_go += 250;
				break;
			case MapTile::Bonus_None:
			case MapTile::Bonus_Random:
				break;
			default: // bad bonuses
				_graph_stub->nodes[i].score_to_go -= 300;
				break;
		}


		// edges:

		// right 
		if (i % cols < cols-1)	{
			MapTile *t = _field->getMap()->getTile(i+1);
			_graph_stub->nodes[i+1].is_accessible &= t->walkeable();
		}

		// left 
		if (i % cols > 0) {
			MapTile *t = _field->getMap()->getTile(i-1);
			_graph_stub->nodes[i-1].is_accessible &= t->walkeable();
		}

		//  up
		if (i >= cols) {
			MapTile *t = _field->getMap()->getTile(i-cols);
			_graph_stub->nodes[i-cols].is_accessible &= t->walkeable();
		}

		//  down
		if (i < tilesCount - cols) {
			MapTile *t = _field->getMap()->getTile(i+cols);
			_graph_stub->nodes[i+cols].is_accessible &= t->walkeable();
		}

	}

}

MapPathFinderGraph* MapPathFinder::getGraph()
{
	MapPathFinderGraph *graph = new MapPathFinderGraph(_graph_stub);

	return graph;	
}

void MapPathFinder::searchPathes(MapPathFinderGraph *graph)
{
	// simple array, nodes will be upto 500 and it is not neccessery to have quicker min-heap
	MapPathFinderGraphNode **Q = new MapPathFinderGraphNode *[graph->nodes_count];

	const int cols = _map->getCols();
	
	// init Q
	for (int i = 0; i < graph->nodes_count; i++)
	{
		Q[i] = &graph->nodes[i];
	}

	// run Dijkstra
	// run dijkstra algorithm
	while (true)
	{
		// get nearest node from Q
		MapPathFinderGraphNode *nearest_node = NULL;
		for (int i = 0; i < graph->nodes_count; i++)
		{
			// Already removed index
			if (Q[i] == NULL)
				continue;

			if (nearest_node == NULL)
			{
				// here can be nodes with both infinite even finite distances
				nearest_node = Q[i];
			}
			else
			{
				if (Q[i]->distance < 0)
				{
					// Node with infinite disatance cannot be nearer than every else.
					// But first iteration (nearest == NULL) ensures that if all nodes are infinite far,
					// at least one of them will be result of nearest search
					continue;
				}
				else if (nearest_node->distance > Q[i]->distance || nearest_node->distance < 0)
				{
					// nearer node found
					nearest_node = Q[i];
				}
			}
		}

		// remove precessed node from Q
		Q[nearest_node->index] = NULL;

		if (nearest_node == NULL || nearest_node->distance < 0)
			break;

		// go throught neigbords of nearest node and relax its distances
		// left
		if (nearest_node->index % cols != 0)
		{
			graph->nodes[nearest_node->index - 1].relaxDistanceIfExistsEdge(nearest_node);
		}
		// right
		if (nearest_node->index % cols < cols - 1)
		{
			graph->nodes[nearest_node->index + 1].relaxDistanceIfExistsEdge(nearest_node);
		}
		// up
		if (nearest_node->index >= cols)
		{
			graph->nodes[nearest_node->index - cols].relaxDistanceIfExistsEdge(nearest_node);
		}
		// down
		if (nearest_node->index < graph->nodes_count - cols)
		{
			graph->nodes[nearest_node->index + cols].relaxDistanceIfExistsEdge(nearest_node);
		}
	}

	delete[] Q;

}

bool MapPathFinder::canPlaceBomb(MapCoord coord, int bomb_power)
{
	MapPathFinderGraph *graph = getGraph();

	// check new explosive tiles
	const int rows = _field->getMap()->getRows();
	const int cols = _field->getMap()->getCols();
	int bomb_index = coord.col + coord.row * cols;

	graph->nodes[bomb_index].explosive = true;
	
	// to left
	for (int d = 1; d <= bomb_power; d++)
	{
		MapTile *t = _field->getMap()->getTile(bomb_index - d);
		if (t == NULL || t->type() != MapTile::Tile_Ground)
			break;
		graph->nodes[bomb_index-d].explosive = true;
	}
	// to right
	for (int d = 1; d <= bomb_power; d++)
	{
		MapTile *t = _field->getMap()->getTile(bomb_index + d);
		if (t == NULL || t->type() != MapTile::Tile_Ground)
			break;
		graph->nodes[bomb_index+d].explosive = true;
	}
	// to top
	for (int d = 1; d <= bomb_power; d++)
	{
		MapTile *t = _field->getMap()->getTile(bomb_index - d*cols);
		if (t == NULL || t->type() != MapTile::Tile_Ground)
			break;
		graph->nodes[bomb_index-d*cols].explosive = true;
	}
	// to bottom
	for (int d = 1; d <= bomb_power; d++)
	{
		MapTile *t = _field->getMap()->getTile(bomb_index + d*cols);
		if (t == NULL || t->type() != MapTile::Tile_Ground)
			break;
		graph->nodes[bomb_index+d*cols].explosive = true;
	}

	// DFS if there is some path to non-explosive tile
	bool *touched = new bool[graph->nodes_count];
	for (int i = 0; i < graph->nodes_count; i++)
	{
		touched[i] = false;
	}

	QStack<int> dfs_stack;
	dfs_stack.push(bomb_index);

	// first DFS-tree is enought - more will be even unaccessible from bomb_index
	while (!dfs_stack.isEmpty())
	{
		int current = dfs_stack.pop();

		if (touched[current])
			continue;

		touched[current] = true;
		if (!graph->nodes[current].explosive)
		{
			return true;
		}

		// push nodes connected with current to dfs stack
		// left
		if (current % cols != 0 && graph->nodes[current].existsEdgeTo(&graph->nodes[current-1])) {
			dfs_stack.push(current-1);
		}
		// right
		if (current % cols < cols - 1 && graph->nodes[current].existsEdgeTo(&graph->nodes[current+1])) {
			dfs_stack.push(current+1);
		}
		// up
		if (current >= cols && graph->nodes[current].existsEdgeTo(&graph->nodes[current-cols])) {
			dfs_stack.push(current-cols);
		}
		// down
		if (current < graph->nodes_count - cols && graph->nodes[current].existsEdgeTo(&graph->nodes[current+cols])) {
			dfs_stack.push(current+cols);
		}
	}

	return false;
}

MapPathFinder::~MapPathFinder()
{
	delete _graph_stub;
}

MapPathFinderGraph::~MapPathFinderGraph()
{
	delete[] this->nodes;
}



