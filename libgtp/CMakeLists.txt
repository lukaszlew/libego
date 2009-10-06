add_library (gtp gtp.cpp gtp_gogui.cpp)

add_boost_cxx_test (gtp_test)
target_link_libraries (gtp_test gtp) 

add_boost_cxx_test (gtp_gogui_test)
target_link_libraries (gtp_gogui_test gtp) 

#TODO install includes as well
#install (TARGETS gtp ${INSTALL_TARGETS_DEFAULT_ARGS})
