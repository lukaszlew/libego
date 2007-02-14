OPT   = -O3 -march=i686 -fomit-frame-pointer  -DNDEBUG -ffast-math -frename-registers -finline-limit=20000 
DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = -O2 -march=i686 -DNDEBUG -ggdb3 -fno-inline 

CFLAGS += -Wall #-static #-Wno-long-long -Wextra -Wno-variadic-macros

GPP    = g++ $(CFLAGS) 

all: engine_opt


engine_debug: board.cpp stack_board.cpp utils.cpp playout.cpp gtp.cpp gtp.h genmove.cpp gtp_board.cpp main.cpp
	$(GPP) $(DEBUG) -o engine_debug gtp.cpp main.cpp

engine_opt:   board.cpp stack_board.cpp utils.cpp playout.cpp gtp.cpp gtp.h genmove.cpp gtp_board.cpp main.cpp
	$(GPP) $(OPT)   -o engine_opt   gtp.cpp main.cpp

engine_prof:  board.cpp stack_board.cpp utils.cpp playout.cpp gtp.cpp gtp.h genmove.cpp gtp_board.cpp main.cpp
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
