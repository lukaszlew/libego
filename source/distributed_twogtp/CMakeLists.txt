set_cxx_flags (FALSE)

# -----------------------------------------------------------------------------
# Qt

find_package (Qt4)

if (NOT QT_FOUND)
  message("\nWarning: Qt4 not found, d2g will not be build.\n")
  return()
endif()

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
# d2g-worker

add_executable (d2g-worker
  main.cpp database.cpp gtp_process.cpp admin.cpp worker.cpp)

target_link_libraries (d2g-worker utils ego gtp ${QT_LIBRARIES})
install (TARGETS d2g-worker ${INSTALL_TARGETS_DEFAULT_ARGS})

# -----------------------------------------------------------------------------
# d2g-admin

add_executable (d2g-admin
  main.cpp database.cpp gtp_process.cpp admin.cpp worker.cpp)

target_link_libraries (d2g-admin utils ego gtp ${QT_LIBRARIES})
install (TARGETS d2g-admin ${INSTALL_TARGETS_DEFAULT_ARGS})

