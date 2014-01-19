#include "ActionBomberman.h"
#include "src/frontend/gamewidget.h"
#include "src/frontend/menuwidget.h"
#include "src/frontend/gameinfo.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QStackedWidget>

ActionBomberman::ActionBomberman()
	: QMainWindow()
{
	setWindowTitle(trUtf8("ActionBomberman by Lukáš Doležal..."));
	setMinimumSize(700,440);

	// center window
	QRect frect = frameGeometry();
	frect.moveCenter(QDesktopWidget().availableGeometry().center());
	move(frect.topLeft());

	_main_widget = new QStackedWidget();
	setCentralWidget(_main_widget);
	
	menuwidget = new MenuWidget(this);
	connect(menuwidget, SIGNAL(play(GameInfo*)), this, SLOT(play(GameInfo*)));

	_main_widget->addWidget(menuwidget);
}

void ActionBomberman::play(GameInfo* settings)
{
	setUpdatesEnabled(false);
	menuwidget->hide();

	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);

	gamewidget = new GameWidget(this);
	connect(gamewidget, SIGNAL(gameOver()), this, SLOT(gameOver()));

//	setCentralWidget(gamewidget);
	gamewidget->setGameInfo(settings);
	gamewidget->setUpdatesEnabled(true);
	gamewidget->show();
	gamewidget->setFocus(Qt::OtherFocusReason);

	_main_widget->addWidget(gamewidget);
	_main_widget->setCurrentIndex(1);
	
	setUpdatesEnabled(true);
	gamewidget->setFocus();
	gamewidget->start();


	
}

void ActionBomberman::gameOver()
{
	setUpdatesEnabled(false);
	gamewidget->hide();

	_main_widget->removeWidget(_main_widget->currentWidget());
	delete gamewidget;

	//menuwidget->show();
	setUpdatesEnabled(true);
	
}



