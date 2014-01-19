/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H
#include <QString>

namespace ABomberman {
class Map;
}

using namespace ABomberman;

class MapGenerator
{

public:
	bool generate_random_bricks;

	// ratio of random generated bonuses. 0 no random bonuses, 1 each tile has bonus
	qreal random_bonuses_ratio;

	MapGenerator();

	/// Generate map from given arena file
	Map *fromFile(const QString& filename);
};

#endif // MAPGENERATOR_H
