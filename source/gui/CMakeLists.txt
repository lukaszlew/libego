find_package(Qt4 REQUIRED)
include (${QT_USE_FILE})

include_directories (
  ${libego_SOURCE_DIR}/engine
  ${libego_SOURCE_DIR}/goboard
  ${libego_SOURCE_DIR}/utils
  ${libego_SOURCE_DIR}/gtp
  ${libego_SOURCE_DIR}/libgamegui
)  

add_definitions ( -Wall )

qt4_wrap_cpp (MOC_SRCS manager.h)
add_library (gui gui.cpp manager.cpp ${MOC_SRCS})
