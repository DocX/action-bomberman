/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_MAPPATHFINDER_H
#define ABOMBERMAN_MAPPATHFINDER_H
#include <qlinkedlist.h>

template<class T >
class QLinkedList;
template<class T >
class QLinkedList;
namespace ABomberman {

class MapCoord;


class GameField;


class Map;

/**
 * Class representing graph node in path-search algorithms. It stores
 * information about itself, neighbors. Provide weighting function.
 * 
 */
class MapPathFinderGraphNode
{
public:
	/**
	 * Constructs empty node with default parameters
	 */
	MapPathFinderGraphNode();

	/**
	 * Constructs new node with some parameters copied from given
	 * node. Distance, previous node in path and other properties that
	 * should be cleared for new path-search algorithm run is cleared.
	 *
	 * @param node Node from which is copied some parameters
	 */
	MapPathFinderGraphNode(MapPathFinderGraphNode *node);
	
	/// Relax node's distance using edge from given node
	void relaxDistance(MapPathFinderGraphNode* from);

	/// Returns if on base of given node properties exists edge to given node
	bool existsEdgeTo(ABomberman::MapPathFinderGraphNode* to);

	/// If given node's properties implies existance of edge, then relax distance
	void relaxDistanceIfExistsEdge(MapPathFinderGraphNode* from);
	
	/// "distance" from source used in Dijkstra's algorithm. Infinity = -1
	qreal distance;

	/// Real distance in count of tiles
	int real_distance;

	/// Node previous in shortest path
	int path_previous_node_index;

	/// Index of corresponding Tile
	int index;

	/// if node (tile) is in dangerous of explosion
	bool explosive;

	/// If tile is exploding
	bool exploding;

	/// If explosive, number of mseconds remaining to expolosion on this tile
	int explode_timeout;

	/// If node can be accessible by edge. This have highest priority.
	bool is_accessible;

	/// True if path to this node is throught explosive tile
	bool path_is_dangerous;
	
	/// How much is this node atactive to go there.
	int score();

	/// score to placing bomb (bricks, enemies..)
	int score_place_bomb;
	/// score to go there (bonuses..)
	int score_to_go;
};

/// Class representing graph used for path search and as its result
class MapPathFinderGraph
{
public:
	/**
	 * Constructs graph with array of given number of nodes
	 */
	MapPathFinderGraph(int nodes);

	/**
	 * Deep copy constructor. Creates new instance of nodes list and each node is
	 * also copied by copy-constructor
	 */
	MapPathFinderGraph(MapPathFinderGraph *graph);

	/// Array of nodes
	MapPathFinderGraphNode *nodes;

	/// Count of nodes (vertexes) in graph
	int nodes_count;

	/**
	 * Destructor deletes nodes
	 */
	~MapPathFinderGraph();

};

/**
 * Class encapsulating methods used by ComputerPlayer to find best path.
 * Using Dijkstra's algorithm to search pathes.
 * It stores graph stub for quicker processing by more players in one tick.
 */
class MapPathFinder
{
public:

	/**
	 * Constructs empty finder.
	 *
	 * @param map Pointer to game map instance used to initialize graph nodes
	 * @param field Pointer to game field used to initialize graph nodes
	 */
	MapPathFinder(Map *map, GameField *field);

	/// Prepare graph data using current state of map and field and save it to graph stub
	void InitializeGraph();

	/**
	 * Search pathes in given graph using Dijkstra's algorithm
	 * Graph's nodes is alternated in given instance.
	 */
	void searchPathes(MapPathFinderGraph *graph);

	/// Determine if exists escape path from given coord as if there will be bomb
	bool canPlaceBomb(MapCoord coord, int bomb_power);
	
	/// Creates copy of graph stub for use in search
	MapPathFinderGraph *getGraph();

	~MapPathFinder();
	
private:
	/// pointer to game map
	Map *_map;

	/// pointer to game field
	GameField *_field;

	/// stub Graph instance used for initializing graph in each search
	MapPathFinderGraph *_graph_stub;
};

}

#endif // ABOMBERMAN_MAPPATHFINDER_H
