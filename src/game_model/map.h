/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ABOMBERMAN_MAP_H
#define ABOMBERMAN_MAP_H
//#include "gamefield.h"
//#include "maptile.h"
#include <QtGlobal>
#include <QList>


class QPointF;
namespace ABomberman {

class GameField;

/**
 * MapCoord defines row and column on map and can operate with it.
 */
struct MapCoord
{
	/**
	 * Creates MapCoord instance on zero point (0,0)
	 */
	MapCoord()
	{
		MapCoord(0,0);
	};

	/**
	 * Creaes MapCoord with given coordinates
	 *
	 * @param row number of row to save
	 * @param col number of columnt to save
	 */
	MapCoord(int row, int col)
	{
		this->row = row;
		this->col = col;
	};

	/**
	 * Returns coordinations of center of representing tile for objects coordinate system
	 */
	QPointF toPoint();
	
	int row;
	int col;

	/**
	 * Adds number of rows and columns to current numbers
	 *
	 * @param rows number of rows to add
	 * @param cols number of columns to add
	 */
	MapCoord add(int rows, int cols);
};

class MapTile;

/**
 * Map encapsulates list of tiles and provide some selecting methods on it.
 */
class Map
{
public:
	/**
	 * Constructs new Map instance with given size and default tiles (Ground without bonuses)
	 *
	 * @param rows number of rows
	 * @param cols number of columns
	 */
	Map(int rows, int cols);

	/**
	 * Deep copy constructor of other map. Creates new instances of tiles and copy its properties
	 *
	 * @param map_to_copy instance of map to copy
	 */
	Map(const Map* map_to_copy);
	
	~Map();

	/// Returns tile at given index (row*cols+col) or NULL if bad index is given.
	MapTile *getTile(int index);
	
	/// Returns tile at given row and column. NULL if bad coordinates passed.
	MapTile *getTile(int row, int col);
	
	/// Returns tile at given real position. NULL if bad coordinates passed.
	MapTile *getTile(qreal x, qreal y);

	/// Returns number of rows
	int getRows();

	/// Returns number of columns
	int getCols();

	/// Returns number of tiles in total
    int tilesCount();

	/// Return index in internal list of given MapTile instance
	int getTileIndex(MapTile *);

	/**
	 * Returns coordination of starting position with given index.
	 * If index is greater than number of positions, it is rolled using
	 * modulo.
	 */
	MapCoord getStartingPosition(int number);

	/**
	 * Copy list of coordination to internal list of starting positions
	 */
	void setStartingPositions(const QList<MapCoord> &positions);

	/**
	 * Returns width in objects coordination system. Equal to columns * tileWidth.
	 */
	qreal getRealWidth();

	/**
	 * Return s height of map in objects coordination system. Equal to rows * tileWidth
	 */
	qreal getRealHeight();
	
private:
	int rows;
	int cols;
	
	/// Array of tiles
	MapTile *mapTiles;

	/// List of startings positions
	QList<MapCoord> startingPositions;
	
};

}

#endif // ABOMBERMAN_MAP_H
