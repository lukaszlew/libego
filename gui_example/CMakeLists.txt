set ( SRCS main.cpp gogui.cpp manager.cpp)
set ( MOC_HDRS gogui.h manager.h)
set ( RCS )
set ( UIS )

set ( BOOST_ROOT /usr/local/include/boost-1_39 )
add_definitions ( -Wall )

find_package(Qt4 REQUIRED)
include ( ${QT_USE_FILE} )
qt4_add_resources ( RC_SRCS ${RCS} )
qt4_wrap_ui ( UI_HDRS ${UIS} )
qt4_wrap_cpp ( MOC_SRCS ${MOC_HDRS} )

include_directories (
  ${BoardGameGui_SOURCE_DIR}/libgamegui
  ${CMAKE_BINARY_DIR}
  ${BOOST_ROOT}
)  

link_directories ( ${BoardGameGui_BINARY_DIR}/libgamegui )

add_executable (gui_example  ${SRCS} ${RC_SRCS} ${UI_HDRS} ${MOC_SRCS} )

target_link_libraries(gui_example gamegui ${QT_LIBRARIES})

