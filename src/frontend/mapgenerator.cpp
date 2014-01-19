/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "mapgenerator.h"
#include <qfile.h>
#include <qtextstream.h>
#include "../game_model/map.h"
#include "../game_model/maptile.h"
#include <QDebug>
#include <qstringlist.h>

MapGenerator::MapGenerator()
{
	generate_random_bricks = false;
	random_bonuses_ratio = 0.0;
}

Map* MapGenerator::fromFile(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		std::bad_exception e;

		throw e;
	}

	QTextStream reader(&file);
	QString line;

	int rows, cols;
	
	reader >> rows >> cols;

	qDebug() << "Rows" << rows << "Cols" << cols;
	reader.readLine();
	//qDebug() << "Line" << line;

	Map *map = new Map(rows, cols);

	int i = 0;
	while (i < rows*cols)
	{
		line = reader.readLine();
		QStringList cells_desc = line.split(" ");

		while (!cells_desc.isEmpty())
		{
			MapTile *tile = map->getTile(i);

			// cell type
			QCharRef ch = cells_desc.first()[0];
			tile->setType(ABomberman::MapTile::Tile_Ground);
			if (ch == 'W')
				tile->setType(ABomberman::MapTile::Tile_Wall);
			else if (ch == 'G')
				tile->setType(ABomberman::MapTile::Tile_Ground);
			else if (ch == 'B')
				tile->setType(ABomberman::MapTile::Tile_Brick);
			else if (ch == 'R')
				// if enabled set brick or ground randomly on taged tiles, otherwise brick will be choosed always
				tile->setType(!generate_random_bricks || qrand() % 2 == 0 ? ABomberman::MapTile::Tile_Brick : ABomberman::MapTile::Tile_Ground);
 
			// bonus
			ch = cells_desc.first()[1];
			// if enabled, generate random bonus. If some is seted in file, bonus will be replaced by it,
			if (tile->type() == MapTile::Tile_Brick && random_bonuses_ratio > 0 && qrand() % (int)(1/random_bonuses_ratio) == 0)
			{
				tile->setBonus(MapTile::Bonus_Random);
				tile->setBonus(tile->getBonus());
			}
			else
			{
				tile->setBonus(MapTile::Bonus_None);
			}
			
			if (ch == 'S')
				tile->setBonus(ABomberman::MapTile::Bonus_Speed);
			else if (ch == 'B')
				tile->setBonus(ABomberman::MapTile::Bonus_Bomb);
			else if (ch == 'P')
				tile->setBonus(ABomberman::MapTile::Bonus_MorePower);
			else if (ch == 'K')
				tile->setBonus(ABomberman::MapTile::Bonus_CanKick);
			else if (ch == 'R')
				tile->setBonus(ABomberman::MapTile::Bonus_Random);
			else if (ch == 'G')
				tile->setBonus(ABomberman::MapTile::Bonus_RandomGood);
			else if (ch == 'D')
				tile->setBonus(ABomberman::MapTile::Bonus_RandomBad);
			else if (ch == 'L')
				tile->setBonus(ABomberman::MapTile::Bonus_Slowness);
			else if (ch == 'H')
				tile->setBonus(ABomberman::MapTile::Bonus_Hyperactive);
			else if (ch == 'W')
				tile->setBonus(ABomberman::MapTile::Bonus_WetBombs);
			else if (ch == 'I')
				tile->setBonus(ABomberman::MapTile::Bonus_Shield);

			i++;
			cells_desc.removeFirst();
		}
	}

	// reads starting positions

	int startingPositionsCount;
	reader >> startingPositionsCount;
	
	QList<MapCoord> startingPositions;
	for(int i = 0; i < startingPositionsCount; i++)
	{
		int x, y;
		reader >> y;
		reader >> x;

		if (x < 0 || x >= cols || y < 0 || y >= rows)
		{
			qDebug() << "bad starting position";
			std::bad_exception e;
			throw e;
		}
		
		startingPositions.append(MapCoord(y,x));
		qDebug() << "starting position" << x << y;
	}

	map->setStartingPositions(startingPositions);

	return map;
	// destructor closes file
}


