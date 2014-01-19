/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <qwidget.h>

#include <QSvgRenderer>
#include <QPixmapCache>

class GameInfo;
class PlayerHandler;

namespace ABomberman {
class GameField;
class Map;
}

using namespace ABomberman;



class GameWidget : public QWidget
{
	Q_OBJECT
    int _debug_player;
    qreal point_multiplier;
    int tile_side;
	
	GameField* field;

	GameInfo* settings;
	
    QPixmap _stripe_ground;
    QPixmap _stripe_wall;
	QPixmap _stripe_brick;
	QPixmap _pixmap_background;
    QPixmap _stripe_bomb;
    QPixmap _stripe_bonus_bomb;
    QPixmap _stripe_bonus_power;
    QPixmap _stripe_bonus_speed;
    QPixmap _stripe_bonus_kick;
    QPixmap _stripe_bonus_throw;
    QPixmap _stripe_bonus_slowness;
    QPixmap _stripe_bonus_hyper;
    QPixmap _stripe_bonus_wet;
    QPixmap _stripe_bonus_random;
	QPixmap _stripe_bonus_shield;
	QPixmap _stripe_bonus_random_bad;
	QPixmap _stripe_bonus_random_good;
    QPixmap _stripe_bomb_blast_core;
    QPixmap _stripe_bomb_blast_west;
    QPixmap _stripe_bomb_blast_east;
    QPixmap _stripe_bomb_blast_north;
    QPixmap _stripe_bomb_blast_south;
	
	bool test_draw;
	
	int pointToPixel(int );

	void initPixmap(QPixmap& pixmap, const QString& element_name, const QSize& size, QSvgRenderer* svg, QPainter* p);
	void initGraphics();
	
	static const int repaint_period;
    QTimer* _repaint_timer;

	static const int panel_size_percentage;
    int panel_size;
    QPixmap _stripe_player;
	void paintScene(QPainter* painter);
	void paintExplosion(QPainter* painter, int x, int y, int dist_north, int dist_west, int dist_south, int dist_east);
	void paintPlayer(QPainter* painter, PlayerHandler* player);
    void paintPanel(QPainter* );
    void paintOverlayer(QPainter* arg1);

	int _debug;
	
public:
    GameWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~GameWidget();

	void setGameInfo(GameInfo* setting);

	void start();
	
protected:
    virtual void keyPressEvent(QKeyEvent* );
    virtual void keyReleaseEvent(QKeyEvent* );
    virtual void paintEvent(QPaintEvent* );
    virtual void resizeEvent(QResizeEvent* );
  //  virtual void timerEvent(QTimerEvent* );
	
public slots:
	void repaintTick();
    void roundEnd();
signals:
    void gameOver();
};

#endif // GAMEWIDGET_H
