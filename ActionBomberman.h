/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#ifndef ActionBomberman_H
#define ActionBomberman_H

#include <QObject>
#include <QMainWindow>

class QStackedWidget;
class GameInfo;
class MenuWidget;
class GameWidget;

/**
 * Main window of ActionBomberman game.
 * Handles menu and game widgets and provide cooperation
 * between them
 */
class ActionBomberman : public QMainWindow
{
Q_OBJECT

public:
    ActionBomberman();
private:
	GameWidget *gamewidget;
	MenuWidget *menuwidget;
    QStackedWidget* _main_widget;
public slots:
	/**
	 * start game with given settings. It creates new field and game widget and
	 * place it on top of window
	 */
	void play(GameInfo* settings);

	/**
	 * game is over, destruct game widget and show menu widget
	 */
    void gameOver();
signals:
	
};

#endif // ActionBomberman_H
