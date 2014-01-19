/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "menuwidget.h"
#include "playerhandler.h"
#include "gameinfo.h"
#include "../game_model/map.h"
#include <QMessageBox>
#include <QDirIterator>
#include <QStringListModel>
#include <QDebug>
#include "addplayerdialog.h"
#include <qstandarditemmodel.h>
#include "playeritem.h"
#include "../game_model/computercharecter.h"
#include "mapgenerator.h"

MenuWidget::MenuWidget(QWidget* parent)
	: QWidget(parent)
{
	this->setupUi(this);

	this->setLayout(mainLayout);

	connect(playButton, SIGNAL(clicked(bool)), this, SLOT(playClicked(bool)));
	connect(quitButton, SIGNAL(clicked(bool)), this, SLOT(clickedQuit(bool)));
	connect(addPlayerButton, SIGNAL(clicked(bool)), this, SLOT(clickedAddPlayer(bool)));
	connect(removePlayerButton, SIGNAL(clicked(bool)), this, SLOT(clickedRemovePlayer(bool)));
	connect(editPlayerButton, SIGNAL(clicked(bool)), this, SLOT(clickedEditPlayer(bool)));

	initMapList();

	_playermodel = new QStandardItemModel(playersList);
	_playermodel->setColumnCount(1);
	playersList->setModel(_playermodel);
	
}

void MenuWidget::initMapList()
{
	QDirIterator it("data/arenas");

	QStringListModel *filelist = new QStringListModel(this->mapListView);
	QStringList list;
	
	while (it.hasNext())
	{
		it.next();
		if (!it.fileInfo().isFile())
			continue;
		qDebug() << "Found arena" << it.fileName();
		list << it.fileName();
	}
	
	filelist->setStringList(list);
	qDebug() << "string count" << filelist->stringList().count();
	mapListView->setModel(filelist);
	mapListView->setMovement(QListView::Static);
	mapListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mapListView->setSelectionMode(QAbstractItemView::SingleSelection);
}


void MenuWidget::playClicked(bool )
{
	GameInfo* game = NULL;

	if (!mapListView->selectionModel()->hasSelection())
	{
		QMessageBox box;
		box.setText("Select arena");
		box.exec();
		return;
	}

	if (playersList->model()->rowCount() <= 1)
	{
		QMessageBox box;
		box.setText("Game can start with 2 players at least");
		box.exec();
		return;
	}

	QString arena_filename;
	arena_filename = QString("data/arenas/%1").arg(
		mapListView->selectionModel()->currentIndex().data().toString()
		);

	qDebug() << arena_filename;
	
	try
	{
		MapGenerator mapGenerator;
		mapGenerator.generate_random_bricks = true;
		mapGenerator.random_bonuses_ratio = 0.3;
		game = new GameInfo(mapGenerator.fromFile(arena_filename));
	}
	catch (std::exception e)
	{
		delete game;
		
		QMessageBox box;
		box.setText("Game initialization failed");
		box.exec();
		
		return;
	}

	game->round_time = gameTimeSpinBox->value() * 1000;
	game->wins_to_end = roundsSpinBox->value();
	game->initialBombCount = bombCountSpin->value();
	game->initialBombPower = bombPowerSpin->value();
	game->initialWalkingSpeed = speedSpin->value();

	for(int i = 0; i < _playermodel->rowCount(); i++)
	{
		PlayerHandler *pl = static_cast<PlayerItem*>(_playermodel->item(i))->playerHandler();
		pl->score = 0;
		game->players->append(pl);
	}
	
	
	emit play(game);
}

void MenuWidget::clickedQuit(bool )
{
	QApplication::instance()->exit();
}

void MenuWidget::clickedAddPlayer(bool )
{
	AddPlayerDialog *dialog = new AddPlayerDialog(this);
	connect(dialog, SIGNAL(playerAdded(AddPlayerDialog*)), this, SLOT(createPlayer(AddPlayerDialog*)));
	dialog->nameEdit->setText(QString("Player %1").arg(_playermodel->rowCount()+1));
	dialog->show();		
}

void MenuWidget::clickedEditPlayer(bool )
{
	if (!playersList->selectionModel()->hasSelection())
		return;

	AddPlayerDialog *dialog = new AddPlayerDialog(this);
	connect(dialog, SIGNAL(playerAdded(AddPlayerDialog*)), this, SLOT(changePlayer(AddPlayerDialog*)));

	PlayerHandler *pl = static_cast<PlayerItem*>(_playermodel->item(playersList->selectionModel()->currentIndex().row()))->playerHandler();

	dialog->edited_player = pl;
	
	dialog->nameEdit->setText(pl->name());
	dialog->aiCheckBox->setChecked(pl->computer_player);
	dialog->aiCheckBox->setDisabled(true);
	dialog->colorButton->setCurrentColor(pl->color);
	dialog->leftKeyEdit->setKey(pl->keyLeft());
	dialog->rightKeyEdit->setKey(pl->keyRight());
	dialog->downKeyEdit->setKey(pl->keyDown());
	dialog->upKeyEdit->setKey(pl->keyUp());
	dialog->fireKeyEdit->setKey(pl->keyFire());
	
	dialog->show();

}


void MenuWidget::clickedRemovePlayer(bool )
{
	if (!playersList->selectionModel()->hasSelection())
		return;

	// delete internal PlayerHandler
	delete static_cast<PlayerItem*>(_playermodel->item(playersList->selectionModel()->currentIndex().row()))->playerHandler();
	
	_playermodel->removeRow(
		playersList->selectionModel()->currentIndex().row()
	);
}

void MenuWidget::createPlayer(AddPlayerDialog* dialog)
{
	PlayerHandler *player;
	if (dialog->aiCheckBox->isChecked())
	{
		player = new PlayerHandler(new ComputerCharacter(0,0,NULL));
		player->acceptsKey = false;
		player->computer_player =true;
	}
	else
	{
		player = new PlayerHandler(new Character(0,0,NULL));
	}

	dialog->setPlayerByDialog(player);

	this->_playermodel->appendRow(new PlayerItem(player));

}

void MenuWidget::changePlayer(AddPlayerDialog* dialog)
{
	dialog->setPlayerByDialog(dialog->edited_player);
	for(int i = 0; i < _playermodel->rowCount(); i++)
	{
		PlayerItem *pl = static_cast<PlayerItem*>(_playermodel->item(i));
		pl->setPlayerHandler(pl->playerHandler());
	}
}

/// Finds PlayerHandler in configured players, which have binded given key and returns pointer to it.
/// if noone found, returns NULL
PlayerHandler *MenuWidget::whoBindKey(Qt::Key key)
{
	for(int i = 0; i < _playermodel->rowCount(); i++)
	{
		PlayerHandler *pl = static_cast<PlayerItem*>(_playermodel->item(i))->playerHandler();
		
		if (pl->keyDown() == key || pl->keyFire() == key || pl->keyLeft() == key || pl->keyRight() == key || pl->keyUp() == key)
		{
			return pl;
		}
	}
	return NULL;
}

