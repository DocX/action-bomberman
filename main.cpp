/*
ActionBomberman
Hra ve stylu bombermana s moznosti hry vice hracu a AI.

Zapoctova prace

2010, Lukas Dolezal
*/

#include <QApplication>
#include "ActionBomberman.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    
	ActionBomberman *bomber = new ActionBomberman();
	bomber->show();
	
    return app.exec();
}
