add_library (gtp gtp.cpp gtp_gogui.cpp)

add_boost_cxx_test (gtp_test)
target_link_libraries (gtp_test gtp) 

add_boost_cxx_test (gtp_gogui_test)
target_link_libraries (gtp_gogui_test gtp) 


#install (TARGETS gtp ${INSTALL_TARGETS_DEFAULT_ARGS})
# TODO:
#   make install optional
#   install headers
#   make cmake file for importing
#   install (EXPORT libgtp DESTINATION ?)
#   install (TARGETS gtp EXPORT libgtp ${INSTALL_TARGETS_DEFAULT_ARGS})
