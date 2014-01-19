/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include "addplayerdialog.h"
#include "playerhandler.h"
#include "menuwidget.h"
#include <QMessageBox>

AddPlayerDialog::AddPlayerDialog(MenuWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	menu = parent;
	
	setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	
	setFixedSize(380,400);
	setLayout(mainLayout);

	connect(aiCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggledAi(bool)));
	connect(addButton, SIGNAL(clicked(bool)), this, SLOT(playerAdd(bool)));

	predefinedKeys->addItem("Custom keys", 0);
	predefinedKeys->addItem("Arrow keys + space", 1);
	predefinedKeys->addItem("RFDG + Q", 2);
	predefinedKeys->setCurrentIndex(0);

	connect(predefinedKeys, SIGNAL(currentIndexChanged(int)), this, SLOT(predefinedChanged(int)));

	colorButton->insertColor(Qt::black);
	colorButton->insertColor(Qt::white);
	colorButton->insertColor(Qt::red);
	colorButton->insertColor(Qt::blue);
	colorButton->insertColor(Qt::green);
	colorButton->insertColor(Qt::yellow	);
	colorButton->insertColor(QColor::fromHsl(40,255,200));

	edited_player = NULL;
}


void AddPlayerDialog::toggledAi(bool checked)
{
	setKeysEnabled(!checked);
	predefinedKeys->setEnabled(!checked);

}

void AddPlayerDialog::playerAdd(bool )
{
	// check keys for human players
	if (!aiCheckBox->isChecked())
	{
		if (leftKeyEdit->key == 0 || rightKeyEdit->key == 0 ||
			upKeyEdit->key == 0 || downKeyEdit->key == 0 || fireKeyEdit->key ==0)
		{
			QMessageBox box;
			box.setText("Please enter control keys");
			box.exec();
			return;
		}

		if (menu->whoBindKey(leftKeyEdit->key) != NULL ||
			menu->whoBindKey(rightKeyEdit->key) != NULL ||
			menu->whoBindKey(downKeyEdit->key) != NULL ||
			menu->whoBindKey(upKeyEdit->key) != NULL ||
			menu->whoBindKey(fireKeyEdit->key) != NULL)
		{
			QMessageBox box;
			box.setText("Other player already using some of keys");
			box.exec();
			return;
		}
	}

	emit playerAdded(this);
	this->close();
}

void AddPlayerDialog::setPlayerByDialog(PlayerHandler* player)
{
	player->setKeyDown(downKeyEdit->key);
	player->setKeyUp(upKeyEdit->key);
	player->setKeyLeft(leftKeyEdit->key);
	player->setKeyRight(rightKeyEdit->key);
	player->setKeyFire(fireKeyEdit->key);
	player->color = colorButton->currentColor() ;
	player->setName(nameEdit->text());
}


void AddPlayerDialog::predefinedChanged(int number)
{
	switch(number)
	{
		case 1:
			// first check keys
			if (menu->whoBindKey(Qt::Key_Left) != NULL ||
				menu->whoBindKey(Qt::Key_Right) != NULL ||
				menu->whoBindKey(Qt::Key_Up) != NULL ||
				menu->whoBindKey(Qt::Key_Down) != NULL ||
				menu->whoBindKey(Qt::Key_Space) != NULL)
			{
				QMessageBox box;
				box.setText("Other player already using some of keys");
				box.exec();
				predefinedKeys->setCurrentIndex(0);
				return;
			}
			
			leftKeyEdit->setKey(Qt::Key_Left);
			rightKeyEdit->setKey(Qt::Key_Right);
			upKeyEdit->setKey(Qt::Key_Up);
			downKeyEdit->setKey(Qt::Key_Down);
			fireKeyEdit->setKey(Qt::Key_Space);
			break;
		case 2:
			// first check keys
			if (menu->whoBindKey(Qt::Key_D) != NULL ||
				menu->whoBindKey(Qt::Key_G) != NULL ||
				menu->whoBindKey(Qt::Key_R) != NULL ||
				menu->whoBindKey(Qt::Key_F) != NULL ||
				menu->whoBindKey(Qt::Key_Q) != NULL)
			{
				QMessageBox box;
				box.setText("Other player already using some of keys");
				box.exec();
				predefinedKeys->setCurrentIndex(0);
				return;
			}
			leftKeyEdit->setKey(Qt::Key_D);
			rightKeyEdit->setKey(Qt::Key_G);
			upKeyEdit->setKey(Qt::Key_R);
			downKeyEdit->setKey(Qt::Key_F);
			fireKeyEdit->setKey(Qt::Key_Q);
			break;
	}

	if (number > 0)
		setKeysEnabled(false);
	else
		setKeysEnabled(true);
}

void AddPlayerDialog::setKeysEnabled(bool checked)
{
	leftKeyEdit->setEnabled(checked);
	rightKeyEdit->setEnabled(checked);
	upKeyEdit->setEnabled(checked);
	downKeyEdit->setEnabled(checked);
	fireKeyEdit->setEnabled(checked);
}


