TEMPLATE = app
TARGET = GoGui
QT += core \
    gui \
    svg
HEADERS += SquareGrid.h \
    Grid.h \
    SquareBoard.h \
    HexBoard.h \
    HexField.h \
    SquareField.h \
    Board.h \
    Field.h \
    gogui.h
SOURCES += SquareGrid.cpp \
    Grid.cpp \
    SquareBoard.cpp \
    HexBoard.cpp \
    HexField.cpp \
    SquareField.cpp \
    Board.cpp \
    Field.cpp \
    main.cpp \
    gogui.cpp
FORMS += 
RESOURCES += images.qrc
CONFIG += debug
