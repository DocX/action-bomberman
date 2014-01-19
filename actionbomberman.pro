
QT += core svg
TEMPLATE = app
TARGET = ActionBomberman

SOURCES += \
./src/frontend/gameinfo.cpp \
./src/frontend/keyeditwidget.cpp \
./src/frontend/mapgenerator.cpp \
./src/frontend/playerhandler.cpp \
./src/frontend/gamewidget.cpp \
./src/frontend/addplayerdialog.cpp \
./src/frontend/menuwidget.cpp \
./src/frontend/playeritem.cpp \
./src/qtcolorpicker/qtcolorpicker.cpp \
./src/game_model/bomb.cpp \
./src/game_model/gameobject.cpp \
./src/game_model/mappathfinder.cpp \
./src/game_model/character.cpp \
./src/game_model/map.cpp \
./src/game_model/maptile.cpp \
./src/game_model/computercharecter.cpp \
./src/game_model/gamefield.cpp \
./ActionBomberman.cpp \
./main.cpp

HEADERS += \
./ActionBomberman.h \
./src/frontend/addplayerdialog.h \
./src/frontend/gameinfo.h \
./src/frontend/gamewidget.h \
./src/frontend/mapgenerator.h \
./src/frontend/keyeditwidget.h \
./src/frontend/playeritem.h \
./src/frontend/menuwidget.h \
./src/frontend/playerhandler.h \
./src/qtcolorpicker/qtcolorpicker.h \
./src/game_model/computercharecter.h \
./src/game_model/mappathfinder.h \
./src/game_model/map.h \
./src/game_model/gameobject.h \
./src/game_model/bomb.h \
./src/game_model/character.h \
./src/game_model/gamefield.h \
./src/game_model/maptile.h

FORMS += \
./src/frontend/addplayerdialog_ui.ui \
./src/frontend/menuwidget_ui.ui
