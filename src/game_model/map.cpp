/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "map.h"
#include <math.h>
#include "maptile.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qstringlist.h>
#include <qpoint.h>
#include "gamefield.h"
#include "gameobject.h"

using namespace ABomberman;

QPointF MapCoord::toPoint()
{
	return QPointF(col*MapTile::SIDESIZE, row*MapTile::SIDESIZE);
}

MapCoord MapCoord::add(int rows, int cols)
{
	MapCoord c(this->row,this->col);
	c.col += cols;
	c.row += rows;

	return c;
}

Map::Map(int rows, int cols) :
	startingPositions()
{
	this->rows = rows;
	this->cols = cols;
	this->mapTiles = new MapTile[rows*cols];
	
	for(int i = 0; i < rows*cols; i++)
	{
		mapTiles[i] = MapTile(MapCoord(i/cols, i%cols));
	}
}


// copy constructor
Map::Map(const ABomberman::Map* map_to_copy)
{
	this->rows = map_to_copy->rows;
	this->cols = map_to_copy->cols;
	this->mapTiles = new MapTile [rows*cols];

	for(int i = 0; i < rows*cols; i++)
	{
		mapTiles[i] = MapTile(MapCoord(i/cols, i%cols));
		mapTiles[i].setType(map_to_copy->mapTiles[i].type());
		mapTiles[i].setBonus(map_to_copy->mapTiles[i].haveBonus());
	}
	
	this->startingPositions = QList<MapCoord>(map_to_copy->startingPositions);
}


int Map::tilesCount()
{
	return rows * cols;
}

void Map::setStartingPositions(const QList< MapCoord >& positions)
{
	startingPositions = QList<MapCoord>(positions);
}


MapCoord Map::getStartingPosition(int number)
{
	number = number % startingPositions.count();
	
	return startingPositions[number];
}

int ABomberman::Map::getRows()
{
	return rows;
}

int Map::getCols()
{
	return cols;
}

qreal Map::getRealHeight()
{
	return (rows-1) * MapTile::SIDESIZE;
}

qreal Map::getRealWidth()
{
	return (cols-1) * MapTile::SIDESIZE;
}


MapTile* Map::getTile(int index)
{
	if (index < 0 || index >= cols*rows)
		return NULL;
	
	return &this->mapTiles[index];
}

MapTile* Map::getTile(qreal x, qreal y)
{
	MapCoord c;
	c.col = (int)floor((x+ (MapTile::SIDESIZE/2.0)) / MapTile::SIDESIZE);
	c.row = (int)floor((y+ (MapTile::SIDESIZE/2.0)) / MapTile::SIDESIZE);
	
	return getTile(c.row, c.col);
}

MapTile* Map::getTile(int row, int col)
{
//	if (col < 0 || row < 0 || col >= cols || row >= rows)
//		return NULL;
	
	return getTile(row*cols + col);
	
}

int Map::getTileIndex(MapTile* tile)
{
	MapCoord coords = tile->coords();
	return coords.row * cols + coords.col;
}



ABomberman::Map::~Map()
{
	delete[] mapTiles;
	
}
