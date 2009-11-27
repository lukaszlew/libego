set_cxx_flags (FALSE)

# -----------------------------------------------------------------------------
# Qt

find_package (Qt4 REQUIRED)
set (QT_USE_QTSQL 1)
set (QT_USE_QTNETWORK 1)
set (QT_DONT_USE_QTGUI 1)
include (${QT_USE_FILE})

include_directories (${QT_INCLUDE_DIRS})
link_directories    (${QT_LIBRARY_DIRS})

# -----------------------------------------------------------------------------
# my libs

# IMPROVE: utils should be imported automatically from libego
include_directories (${libego_SOURCE_DIR}/utils)
link_directories    (${libego_BINARY_DIR}/utils)

include_directories (${libego_SOURCE_DIR}/libego)
link_directories    (${libego_BINARY_DIR}/libego)

include_directories (${libego_SOURCE_DIR}/libgtp)
link_directories    (${libego_BINARY_DIR}/libgtp)

# -----------------------------------------------------------------------------
# distributed_twogtp

add_executable (distributed_twogtp
  main.cpp database.cpp gtp_process.cpp admin.cpp worker.cpp)

target_link_libraries (distributed_twogtp utils ego gtp ${QT_LIBRARIES})
install (TARGETS distributed_twogtp ${INSTALL_TARGETS_DEFAULT_ARGS})
