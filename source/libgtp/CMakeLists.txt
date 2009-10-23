add_library (gtp gtp.cpp gtp_gogui.cpp)

add_boost_cxx_test (gtp_test)
target_link_libraries (gtp_test gtp) 

add_boost_cxx_test (gtp_gogui_test)
target_link_libraries (gtp_gogui_test gtp) 

install (TARGETS gtp ${INSTALL_TARGETS_DEFAULT_ARGS})
# TODO install headers
# TODO make cmake file for importing
