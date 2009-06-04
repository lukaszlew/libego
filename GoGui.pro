TEMPLATE = app
TARGET = GoGui
QT += core \
    gui \
    svg
HEADERS += HexGrid.h \
    BoardView.h \
    BoardScene.h \
    SquareGrid.h \
    Grid.h \
    SquareBoard.h \
    HexBoard.h \
    HexField.h \
    SquareField.h \
    Field.h \
    gogui.h
SOURCES += HexGrid.cpp \
    BoardView.cpp \
    BoardScene.cpp \
    SquareGrid.cpp \
    Grid.cpp \
    SquareBoard.cpp \
    HexBoard.cpp \
    HexField.cpp \
    SquareField.cpp \
    Field.cpp \
    main.cpp \
    gogui.cpp
FORMS += 
RESOURCES += images.qrc
CONFIG += debug
