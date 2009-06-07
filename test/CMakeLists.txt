set ( SRCS main.cpp gogui.cpp )
set ( MOC_HDRS gogui.h )
set ( RCS )
set ( UIS )

add_definitions ( -Wall )

find_package(Qt4 REQUIRED)
include ( ${QT_USE_FILE} )
qt4_add_resources ( RC_SRCS ${RCS} )
qt4_wrap_ui ( UI_HDRS ${UIS} )
qt4_wrap_cpp ( MOC_SRCS ${MOC_HDRS} )

include_directories (
  ${gogui_SOURCE_DIR}/libgogui
  ${CMAKE_BINARY_DIR}
)  

link_directories ( ${gogui_BINARY_DIR}/libgogui )

add_executable (test  ${SRCS} ${RC_SRCS} ${UI_HDRS} ${MOC_SRCS} )

target_link_libraries(test gogui ${QT_LIBRARIES})

