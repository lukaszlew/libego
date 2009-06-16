set ( SRCS GameScene.cpp Field.cpp Grid.cpp ResizableView.cpp HexGrid.cpp SquareGrid.cpp Ruler.cpp YGrid.cpp HoGrid.cpp HavannahGrid.cpp )
set ( MOC_HDRS GameScene.h ResizableView.h )
set ( RCS images.qrc )
set ( UIS )

set ( BOOST_ROOT /usr/local/include/boost-1_39 )
set ( QT_USE_QTSVG TRUE )

add_definitions ( -Wall )

find_package(Qt4 REQUIRED)
include ( ${QT_USE_FILE} )
qt4_add_resources ( RC_SRCS ${RCS} )
qt4_wrap_ui ( UI_HDRS ${UIS} )
qt4_wrap_cpp ( MOC_SRCS ${MOC_HDRS} )

include_directories ( ${CMAKE_BINARY_DIR} ${BOOST_ROOT} )

add_library(gamegui ${SRCS} ${RC_SRCS} ${UI_HDRS} ${MOC_SRCS} )

target_link_libraries(gamegui ${QT_LIBRARIES})
