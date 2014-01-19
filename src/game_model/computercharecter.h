/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_COMPUTERCHARECTER_H
#define ABOMBERMAN_COMPUTERCHARECTER_H

#include "character.h"
#include <QList>

namespace ABomberman {

class MapPathFinder;


class MapPathFinderGraph;


class Bomb;

class ComputerCharacter : public Character
{

public:
	/**
	 * Constructs ComputerCharacter instance at given position
	 *
	 * @param initX X coordinate of position
	 * @param initY Y coordnita of position
	 */
    ComputerCharacter(qreal initX, qreal initY, GameField* parentField);
	
    virtual void update();
    virtual void react();

	/**
	 * Pointer to previously computed path-graph
	 */
    ABomberman::MapPathFinderGraph* latest_path;

	/// node index of next node on the path to the destination
	int previous_next_node_index;

	/// stores node where can't go. This is useful when some node have best score but is disallowed to place a bomb.
	/// When next node is reached, this is reseted
	int previous_disallowed_node;

	/// Index of destination node
	int previous_destination_index;

	/**
	 * Overriden reset method. Initialize internal values.
	 */
    virtual void reset();
	
private:

	/// number of update cycles estimated to next recompute
    int recompute_countdown;

	/// Compute and change direction where to go
	void recomputeDirection();

	/// Set individual graph properties and determine pathes using field's finder
	MapPathFinderGraph *searchMyPathes();

};

}

#endif // ABOMBERMAN_COMPUTERCHARECTER_H
