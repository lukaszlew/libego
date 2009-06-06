set ( QT_USE_QTSVG TRUE )

set ( SRCS 
  BoardScene.cpp  
  Field.cpp  
  Grid.cpp      
  HexField.cpp  
  SquareField.cpp
  BoardView.cpp   
  HexBoard.cpp  
  HexGrid.cpp   
  SquareBoard.cpp  
  SquareGrid.cpp 
)

set ( MOC_HDRS 
  BoardScene.h  
  BoardView.h  
  HexBoard.h  
  SquareBoard.h  
)

set ( RCS 
  images.qrc 
)

set ( UIS 
)

add_definitions ( -Wall )

find_package(Qt4 REQUIRED)

include ( ${QT_USE_FILE} )

qt4_add_resources ( RC_SRCS ${RCS} )
qt4_wrap_ui ( UI_HDRS ${UIS} )
qt4_wrap_cpp ( MOC_SRCS ${MOC_HDRS} )


include_directories ( ${CMAKE_BINARY_DIR} )

add_library(gogui ${SRCS} ${RC_SRCS} ${UI_HDRS} ${MOC_SRCS} )

