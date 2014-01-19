/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "gamewidget.h"
#include <QPainter>
#include <QBrush>
#include <QPaintEvent>
#include <qevent.h>
#include <QDebug>
#include <QString>
#include <QPainter>
#include <QDate>
#include <QApplication>
#include <QLayout>

#include "../game_model/maptile.h"
#include "../game_model/gamefield.h"
#include "playerhandler.h"
#include "gameinfo.h"
#include "../game_model/computercharecter.h"
#include "../game_model/mappathfinder.h"


const int GameWidget::repaint_period = 25;

const int GameWidget::panel_size_percentage = 9;

GameWidget::GameWidget(QWidget* parent, Qt::WindowFlags f) :
	QWidget(parent, f),
	settings(NULL),
	field(NULL)
{
	setMinimumSize(480,300);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(false);
	
	
	//repaint timer
	_repaint_timer = new QTimer(this);
	_repaint_timer->stop();
	connect(_repaint_timer, SIGNAL(timeout()), this, SLOT(repaintTick()) );
	
	initGraphics();

	_debug = _debug_player = 0;
}

void GameWidget::setGameInfo(GameInfo* setting)
{
	delete field;
	settings = setting;
	field = new GameField(new Map(settings->map));
	settings->startGame(field);

	initGraphics();

	connect(field, SIGNAL(roundEnd()), this, SLOT(roundEnd()));
}

void GameWidget::start()
{
	if (field != NULL)
		field->start();
}


inline void GameWidget::initPixmap(QPixmap& pixmap, const QString& element_name, const QSize& size, QSvgRenderer* svg, QPainter* p)
{
	pixmap = QPixmap(size);
	pixmap.fill(Qt::transparent);
	p->begin(&pixmap);
	svg->render(p, element_name, pixmap.rect());
	p->end();
}

void GameWidget::initGraphics()
{
	if (settings == NULL) return;
	
	_repaint_timer->stop();
	//bool paused = field->paused();
	field->togglePause(true);
	
	QSvgRenderer svg(QString( "data/graphics.svgz"), this);

	panel_size = (int)(this->height() * (panel_size_percentage * 0.01));
	
	this->tile_side = qMin<int>(this->width() / field->getMap()->getCols(), (this->height() - panel_size) / field->getMap()->getRows());
	this->point_multiplier = tile_side / (qreal)MapTile::SIDESIZE;
	
	QPainter p;

	_pixmap_background =  QPixmap(QSize(this->width(), this->height()));
	p.begin(&_pixmap_background);
	svg.render(&p, "background", QRectF(0,0,this->width(), this->height())); // background is under panel
	p.end();

	initPixmap(_stripe_ground, "arena_ground", QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_wall, "arena_wall",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_brick, "arena_block",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_bomb, "bonus_bomb",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_power, "bonus_power",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_random, "bonus_neutral_pandora",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_speed, "bonus_speed",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_kick, "bonus_kick",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_slowness, "bonus_bad_slow",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_hyper, "bonus_bad_hyperactive",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_wet, "bonus_bad_restrain",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_shield, "bonus_shield",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_random_bad, "bonus_bad_pandora",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bonus_random_good, "bonus_pandora",QSize(tile_side,tile_side), &svg, &p);
	//initPixmap(_stripe_bonus_throw, "bonus_throw",QSize(tile_side,tile_side), &svg, &p);
	initPixmap(_stripe_bomb, "bomb",QSize(tile_side,tile_side), &svg, &p);

	initPixmap(_stripe_bomb_blast_core, "bomb_blast_core_3",QSize(tile_side*1.5,tile_side*1.5), &svg, &p);
	initPixmap(_stripe_bomb_blast_west, "bomb_blast_west_3",QSize(tile_side*1.5,tile_side), &svg, &p);
	initPixmap(_stripe_bomb_blast_east, "bomb_blast_east_3",QSize(tile_side*1.5,tile_side), &svg, &p);
	initPixmap(_stripe_bomb_blast_north, "bomb_blast_north_3",QSize(tile_side,tile_side*1.5), &svg, &p);
	initPixmap(_stripe_bomb_blast_south, "bomb_blast_south_3",QSize(tile_side,tile_side*1.5), &svg, &p);

	foreach (PlayerHandler *player, *settings->players)
	{
		QImage i(tile_side, tile_side, QImage::Format_ARGB32);
		i.fill(Qt::transparent);
		p.begin(&i);
		// render contour
		svg.render(&p,"character",i.rect());

		// blend players color
		p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
		QColor c(player->color);
		c.setAlpha(100);
		p.fillRect(i.rect(), c);

		p.setCompositionMode(QPainter::CompositionMode_SourceOver);
		p.end();

		delete player->pix_right;
		player->pix_right = new QPixmap(QPixmap::fromImage(i));
		p.begin(player->pix_right);	
		// render eyes ;)
		svg.render(&p,"character_eyes_r",i.rect());
		p.end();
		delete player->pix_left;
		player->pix_left = new QPixmap(QPixmap::fromImage(i));
		p.begin(player->pix_left);
		// render eyes ;)
		svg.render(&p,"character_eyes_l",i.rect());
		p.end();


		// blend bomb sprite
		player->pix_bomb = QPixmap(tile_side, tile_side);
		player->pix_bomb.fill(Qt::transparent);
		p.begin(&player->pix_bomb);
		// render contour
		svg.render(&p,"bomb",player->pix_bomb.rect());

		// blend players color
		p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
		c.setAlpha(100);
		p.fillRect(player->pix_bomb.rect(), c);

		p.setCompositionMode(QPainter::CompositionMode_SourceOver);
		p.end();
		
	}
	
	_repaint_timer->start(repaint_period);
	//field->togglePause(paused);
}

void GameWidget::resizeEvent(QResizeEvent* e)
{
	initGraphics();
	update();
    QWidget::resizeEvent(e);
}


void GameWidget::keyPressEvent(QKeyEvent* event)
{
	if (settings == NULL) return;
    switch (event->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if (settings->isEnd())
			{
				// end match
				delete field;
				field = NULL;
				settings = NULL;
				emit gameOver();
			}
			else if (field->currentGameState() == GameField::GameState_GameOver)
			{
				// start new round
				setGameInfo(settings);
				this->start();
			}
			break;
		case Qt::Key_Escape:
			emit gameOver();
			break;
		case Qt::Key_Pause:
#ifdef AI_DEBUG
		case Qt::Key_P:
			field->togglePause(!field->paused());
			break;
		case Qt::Key_B:
			this->_debug = (_debug + 1) % 5;
			break;
#endif
		case Qt::Key_N:
			this->_debug_player = (_debug_player + 1 ) % settings->players->count();
			break;
		default:
			foreach (PlayerHandler *p, *settings->players)
			{
				p->keyPressed(event);
			}
			QWidget::keyPressEvent(event);
			break;
	}
	
}

void GameWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (settings == NULL) return;
	foreach (PlayerHandler *p, *settings->players)
	{
		p->keyReleased(event);
	}
}


inline int GameWidget::pointToPixel(int point)
{
	return (int)(point * this->point_multiplier);
}

void GameWidget::paintEvent(QPaintEvent* ev)
{
//	if (QApplication::instance()->hasPendingEvents())
	//{
	//	QApplication::instance()->processEvents(QEventLoop::ExcludeSocketNotifiers);
	//}
	if (settings == NULL) return;

	//qDebug() << "repainting";
	
	QPainter p(this);

	// background
	p.drawPixmap(0,0,_pixmap_background);

	// panel
	paintPanel(&p);
	
	// align arena
	p.save();
	p.translate(
		(width() - tile_side*field->getMap()->getCols()) / 2,
		((height()-panel_size) - tile_side*field->getMap()->getRows()) / 2 + panel_size
	);
	
	paintScene(&p);

	p.restore();
	paintOverlayer(&p);
}

void GameWidget::paintOverlayer(QPainter* p)
{
	if (field->currentGameState() == GameField::GameState_Playing)
		return;

	
	QRect center_rect(width()/4, height()/6, width()/2, height()/3);
	QColor back(Qt::white);
	back.setAlpha(127);
	QFont f(p->font());
	f.setPixelSize(panel_size/2);
	f.setBold(true);
	p->setFont(f);
	p->setPen(Qt::black);
			p->fillRect(center_rect,back);
	
	switch (field->currentGameState())
	{
		case GameField::GameState_Paused:
			p->drawText(center_rect, Qt::AlignCenter, "PAUSED");
			break;
		case GameField::GameState_GameOver:

			// round result
			PlayerHandler* winner = settings->getRoundWinner();
			if (winner == NULL)
			{
				// draw game
				p->drawText(
					center_rect,
					Qt::AlignCenter,
					QString("DRAW GAME")
				);
			}
			else
			{
				if (settings->isEnd())
				{
					// match winner
					p->drawText(
						center_rect,
						Qt::AlignCenter,
						QString("PLAYER %1 \n WON THE MATCH").arg(winner->name())
					);
				}					
				else
				{
					// winner
					p->drawText(
						center_rect,
						Qt::AlignCenter,
						QString("PLAYER %1 WIN").arg(winner->name())
					);
				}
			}

			f.setPointSize(panel_size/4);
			f.setBold(false);
			p->setFont(f);
			p->drawText(
				center_rect,
				Qt::AlignBottom | Qt::AlignHCenter,
				QString("Press ENTER to continue")
			);
			break;
	}
}


void GameWidget::paintPanel(QPainter* painter)
{
	painter->save();
	
	QColor back(Qt::black);
	back.setAlpha(127);
	
	painter->fillRect(
		QRect(0,0,width(), panel_size),
		back
	);

	// time to end round
	int minutes, seconds;
	const qint32 msecs = field->remindingPlayTime();
	minutes =  msecs / 60000;
	seconds = (field->remindingPlayTime() / 1000) % 60;

	QFont f(painter->font());
	f.setPixelSize(panel_size);
	painter->setFont(f);

	QRect time_rect(
		width() - painter->fontMetrics().averageCharWidth()*5 - 5, 0,
		 painter->fontMetrics().averageCharWidth()*5 , panel_size
		  );

	if (field->currentGameState() != GameField::GameState_GameOver)
	{
		if (minutes == 0 && seconds < 30 && msecs % 500 <= 50) // blink 50 msec in 500msecs
		{
			painter->setPen(Qt::red);
		}
		else
		{
			painter->setPen(Qt::white);
		}

		painter->drawText(
			time_rect,
			Qt::AlignRight | Qt::AlignVCenter,
			QString("%1:%2").arg(minutes).arg(seconds,2)
		);

		// round number
		painter->setPen(Qt::yellow);
		f.setPixelSize(panel_size/2);
		painter->setFont(f);
		painter->drawText(
			QRect(time_rect.left()-200, 0, 200, panel_size),
			Qt::AlignRight | Qt::AlignVCenter,
			QString("Round %1").arg(settings->rounds_played+1)
		);
	}
	else
	{
		painter->setPen(Qt::yellow);
		painter->drawText(
			time_rect,
			Qt::AlignRight | Qt::AlignVCenter,
			"END"
		);


		painter->restore();
		return;
	}

	

	// player stats
	
	f.setPixelSize(panel_size/3);
	painter->setFont(f);

	// max name length is 8 plus some margins and score count
	const int player_box_width = painter->fontMetrics().averageCharWidth()*12;
	
	foreach(PlayerHandler *player, *settings->players)
	{
		painter->setPen(player->color);

		// player name
		f.setBold(true);
		painter->setFont(f);
		painter->drawText(
			QRect(0,0,player_box_width,panel_size),
			Qt::AlignTop | Qt::AlignLeft,
			player->name()
		);
		f.setBold(false);
		painter->setFont(f);
		painter->setPen(Qt::white);
		
		// score
		painter->drawText(
			QRect(0,0,player_box_width,panel_size),
			Qt::AlignTop | Qt::AlignRight,
			QString("#%1").arg(player->score)
		);

		// bomb count
		painter->drawText(
			QRect(0,panel_size/3, player_box_width, panel_size),
			Qt::AlignTop | Qt::AlignLeft,
			QString("B%1").arg(player->player()->bombs())
		);
		// bomb power
		painter->drawText(
			QRect(0,panel_size/3, player_box_width, panel_size),
			Qt::AlignTop | Qt::AlignRight,
			QString("P%1").arg(player->player()->power())
		);
		// shields
		painter->drawText(
			QRect(0,panel_size/3, player_box_width, panel_size),
			Qt::AlignTop | Qt::AlignHCenter,
			QString("S%1").arg(player->player()->shieldsCount())
		);

		// bad bonus countdown
		if (player->player()->badBonusReminding() > 0)
		{
			painter->setPen(Qt::red);
			painter->drawText(
				QRect(0,2*panel_size/3, player_box_width, panel_size),
				Qt::AlignTop | Qt::AlignHCenter,
				QString("BAD %1s").arg(player->player()->badBonusReminding() / 1000)
			);
		}

		// cross if is dead
		if (player->player()->isDead())
		{
			painter->setPen(player->color);
			painter->fillRect(0,0,player_box_width, panel_size, QBrush(Qt::DiagCrossPattern));
		}
		
		
		painter->translate(player_box_width + 5,0);
	}	
	

	painter->restore();
}

void GameWidget::paintScene(QPainter* painter)
{

	// arena
	for(int x = 0; x < field->getMap()->getCols(); x++)
	{
		for (int y = 0; y < field->getMap()->getRows(); y++)
		{
			ABomberman::MapTile *tile = field->getMap()->getTile(y,x);
			QRectF tile_rect = QRectF(x*tile_side, y*tile_side,tile_side, tile_side);
			
			switch (tile->type())
			{
				case MapTile::Tile_Ground:
					switch (tile->haveBonus())
					{
						case MapTile::Bonus_Bomb:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_bomb);
							break;
						case MapTile::Bonus_MorePower:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_power);
							break;
						case MapTile::Bonus_CanKick:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_kick);
							break;
/*						case MapTile::Bonus_CanThrow:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_throw);
							break;
		*/				case MapTile::Bonus_Speed:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_speed);
							break;
						case MapTile::Bonus_Slowness:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_slowness);
							break;
						case MapTile::Bonus_Hyperactive:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_hyper);
							break;
						case MapTile::Bonus_WetBombs:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_wet);
							break;
						case MapTile::Bonus_Random:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_random);
							break;
						case MapTile::Bonus_RandomBad:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_random_bad);
							break;
						case MapTile::Bonus_RandomGood:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_random_good);
							break;
						case MapTile::Bonus_Shield:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_bonus_shield);
							break;
						default:
							painter->drawPixmap(tile_rect.topLeft(), _stripe_ground);
							break;
					}
					break;
				case MapTile::Tile_Wall:
					painter->drawPixmap(tile_rect.topLeft(), _stripe_wall);
					break;
				case MapTile::Tile_Brick:
					painter->drawPixmap(tile_rect.topLeft(), _stripe_brick);
					break;
				
			}

		}
	}

	//render dead players
	QTransform t = painter->worldTransform();
	foreach(PlayerHandler *player, *settings->players)
	{
		if (!player->player()->isDead())
			continue;
		
		painter->translate(
			pointToPixel(player->player()->positionX),
			pointToPixel(player->player()->positionY)
		);
		paintPlayer(painter, player);
		painter->setTransform(t);
	}


	// render objects
	for(int i = 0; i < field->objectsCount(); i++)
	{
		GameObject *obj;
		obj = field->object(i);
		
		if (obj->type == GameObject::BOMB)
		{
			Bomb *bomb = static_cast<Bomb*>(obj);

			if (bomb->state() == Bomb::State_Exploding)
			{
				paintExplosion(painter, pointToPixel(obj->positionX),
							   pointToPixel(obj->positionY), bomb->dist_north, bomb->dist_west, bomb->dist_south, bomb->dist_east);
				
			}
			else
			{
				foreach (PlayerHandler *player, *settings->players)
				{
					if (player->player()->objectID() == bomb->ownerObjectId())
					{
						painter->drawPixmap(pointToPixel(obj->positionX),
									pointToPixel(obj->positionY),
									player->pix_bomb);
						break;
					}
				}		
			}
		}
	}

	
	//render players
	t = painter->worldTransform();
	foreach(PlayerHandler *player, *settings->players)
	{
		if (player->player()->isDead())
			continue;
		
		painter->translate(
			pointToPixel(player->player()->positionX),
			pointToPixel(player->player()->positionY)
		);
		paintPlayer(painter, player);
		painter->setTransform(t);
	}

#ifdef AI_DEBUG
	if (_debug > 0 && settings->players->at(_debug_player)->computer_player)
	{

	for(int x = 0; x < field->getMap()->getCols(); x++)
	{
		for (int y = 0; y < field->getMap()->getRows(); y++)
		{
			ABomberman::MapTile *tile = field->getMap()->getTile(y,x);
			QRectF tile_rect = QRectF(x*tile_side, y*tile_side,tile_side, tile_side);
			// debug
			ComputerCharacter *pl = static_cast<ComputerCharacter *>(settings->players->at(_debug_player)->player());
			if (pl->latest_path != NULL)
			{
				MapPathFinderGraphNode n = pl->latest_path->nodes[x + y*field->getMap()->getCols()];
				if (n.explosive)
				{
					painter->fillRect(tile_rect,QBrush(Qt::black,Qt::CrossPattern));
				}

					if (n.distance >= 0)
					{
						QString str;
						switch (_debug)
						{
							case 1:
								str = QString::number(n.real_distance);
								break;
							case 4:
								str = QString::number(n.distance);
								break;
							case 2:
								str = QString::number(n.score_place_bomb);
								break;
							case 3:
								str = QString::number(n.score_to_go);
								break;
						}
						painter->drawText(tile_rect, Qt::AlignCenter, str);
					}
					else
					{
						painter->drawText(tile_rect, Qt::AlignCenter, "X");
					}
				}
				if (pl->previous_destination_index == x + y*field->getMap()->getCols())
				{
					painter->drawText(tile_rect, Qt::AlignCenter, "#");
				}
			}
		}
	}
#endif

}

void GameWidget::paintPlayer ( QPainter* painter, PlayerHandler* player )
{
	if (player->player()->isDead())
	{
		painter->setOpacity(0.25);
		painter->drawPixmap(0,0,*player->pix_left);
		painter->setOpacity(1.0);
	}
	else
	{
		QPixmap *pix;
		QString t;
		switch (player->player()->direction)
		{
			case GameObject::NORTH:
			case GameObject::EAST:
				pix = player->pix_right;
				break;
			case GameObject::SOUTH:
			case GameObject::WEST:
				pix = player->pix_left;
				break;
		}
#ifdef AI_DEBUG
		switch (player->player()->direction)
		{
			case GameObject::NORTH:
				t = "^";
				break;
			case GameObject::EAST:
				t = ">";
				break;
			case GameObject::SOUTH:
				t = ".";
				break;
			case GameObject::WEST:
				t = "<";
				break;
		}
		
		if (player->player()->isWalking())
			t += "!";
#endif
			
		painter->drawPixmap(0,0,*pix);
		painter->drawText(0,0,t);
	}
}


void GameWidget::paintExplosion(QPainter* painter, int x, int y, int dist_north, int dist_west, int dist_south, int dist_east)
{
	QTransform t = painter->worldTransform();
	painter->translate(x,y);
	

	// transform blast image to 4 direction with
	// appropriate lenght
	
	const qreal quarter_tile = tile_side/4;
	const qreal half_tile = tile_side/2;

	dist_east *= tile_side;
	dist_west *= tile_side;
	dist_south *= tile_side;
	dist_north *= tile_side;
	
	while (dist_north > 0)
	{
		// north
		painter->drawPixmap(0, -dist_north, _stripe_bomb_blast_north);
		dist_north -= tile_side;
	}
	while(dist_west > 0)
	{
		//west
		painter->drawPixmap(-dist_west, 0, _stripe_bomb_blast_west);
		dist_west -= tile_side;
	}
	while (dist_south > 0)
	{
		// south
		painter->drawPixmap(0,dist_south-half_tile, _stripe_bomb_blast_south);
		dist_south -= tile_side;
	}
	while (dist_east > 0)
	{
		// east
		painter->drawPixmap(dist_east-half_tile,0,_stripe_bomb_blast_east);
		dist_east -= tile_side;
	}
	
	// explosion core
	painter->drawPixmap(-quarter_tile, -quarter_tile, _stripe_bomb_blast_core);
	
	painter->setWorldTransform( t);

}



void GameWidget::repaintTick()
{
	//qDebug() << "timer tick";
//	this->enterEvent();
	this->update();
}

void GameWidget::roundEnd()
{
	// determine winner

}


GameWidget::~GameWidget()
{
	delete field;
}


