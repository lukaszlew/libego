set ( SRCS main.cpp manager.cpp gogui.cpp )
set ( EXEC_SRCS game_inc.cpp  )
set ( MOC_HDRS manager.h gogui.h )
set ( RCS )
set ( UIS )

set ( BOOST_ROOT /usr/local/include/boost-1_39 )
set ( ENGINE_MAIN   ${libego_SOURCE_DIR}/engine )
set ( ENGINE_EGO    ${libego_SOURCE_DIR}/goboard )
set ( ENGINE_UTILS  ${libego_SOURCE_DIR}/utils )
set ( ENGINE_GTP    ${libego_SOURCE_DIR}/gtp )
set ( LIBGAMEGUI    ${libego_SOURCE_DIR}/libgamegui )
add_definitions ( -Wall )

find_package(Qt4 REQUIRED)
find_package(Boost COMPONENTS date_time filesystem REQUIRED)
include ( ${QT_USE_FILE} )
qt4_add_resources ( RC_SRCS ${RCS} )
qt4_wrap_ui ( UI_HDRS ${UIS} )
qt4_wrap_cpp ( MOC_SRCS ${MOC_HDRS} )

include_directories (
  ${LIBGAMEGUI}
  ${CMAKE_BINARY_DIR}
  ${BOOST_ROOT}
  ${ENGINE_MAIN}
  ${ENGINE_EGO}
  ${ENGINE_UTILS}
  ${ENGINE_GTP}
)  

link_directories ( ${gogui_BINARY_DIR}/libgogui )


add_library (gui STATIC gui.cpp ${SRCS} ${RC_SRCS} ${UI_HDRDS} ${MOC_SRCS} )


