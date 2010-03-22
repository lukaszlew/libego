set_cxx_flags (FALSE)

include_directories (${libego_SOURCE_DIR}/utils)
include_directories (${libego_SOURCE_DIR}/goboard)
include_directories (${libego_SOURCE_DIR}/gtp)
include_directories (${libego_SOURCE_DIR}/engine)
include_directories (${libego_SOURCE_DIR}/libgamegui)
include_directories (${libego_SOURCE_DIR}/gui)

find_package(Qt4 REQUIRED)
find_package(Boost COMPONENTS date_time filesystem REQUIRED)
include ( ${QT_USE_FILE} )
qt4_add_resources ( RC_SRCS ${RCS} )
qt4_wrap_ui ( UI_HDRS ${UIS} )
qt4_wrap_cpp ( MOC_SRCS ${MOC_HDRS} )

add_executable (engine main.cpp) #TODO make mm_train separate executable
target_link_libraries (engine ai gui gamegui ${QT_LIBRARIES})

install (TARGETS engine ${INSTALL_TARGETS_DEFAULT_ARGS})
