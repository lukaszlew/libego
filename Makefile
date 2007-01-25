OPT   = -O3 -march=i686 -fomit-frame-pointer  -DNDEBUG -ffast-math -frename-registers
DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = $(OPT) -ggdb3 -fno-inline 

CFLAGS += -Wall #-static #-Wno-long-long -Wextra -Wno-variadic-macros

GPP    = g++ $(CFLAGS) 

all: engine_opt



board_test_debug: board.cpp utils.cpp
	$(GPP) $(DEBUG) -DBOARD_TEST -o board_test_debug board.cpp 

board_test_opt:   board.cpp utils.cpp
	$(GPP) $(OPT)   -DBOARD_TEST -o board_test_opt   board.cpp 

board_test_prof:  board.cpp utils.cpp
	$(GPP) $(PROF)  -DBOARD_TEST -o board_test_prof  board.cpp 



playout_test_debug: board.cpp utils.cpp playout.cpp
	$(GPP) $(DEBUG) -DPLAYOUT_TEST -o playout_test_debug playout.cpp 

playout_test_opt:   board.cpp utils.cpp playout.cpp
	$(GPP) $(OPT)   -DPLAYOUT_TEST -o playout_test_opt   playout.cpp 

playout_test_prof:  board.cpp utils.cpp playout.cpp
	$(GPP) $(PROF)  -DPLAYOUT_TEST -o playout_test_prof   playout.cpp 



engine_debug: board.cpp utils.cpp playout.cpp gtp.cpp gtp.h gtp_board.cpp main.cpp
	$(GPP) $(DEBUG) -o engine_debug gtp.cpp main.cpp

engine_opt:   board.cpp utils.cpp playout.cpp gtp.cpp gtp.h gtp_board.cpp main.cpp
	$(GPP) $(OPT)   -o engine_opt   gtp.cpp main.cpp

engine_prof:  board.cpp utils.cpp playout.cpp gtp.cpp gtp.h gtp_board.cpp main.cpp
	$(GPP) $(PROF)  -o engine_prof  gtp.cpp main.cpp



.SUFFIXES: .cpp .o

.cpp.o:
	$(GPP) -c $<

clean:
	rm -f *.s *.o .depend gmon.out core
	rm -f   board_test_debug   board_test_opt   board_test_prof
	rm -f playout_test_debug playout_test_opt playout_test_prof
	rm -f       engine_debug       engine_opt       engine_prof
	rm -f *~
