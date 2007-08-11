OPT   = -O3 -march=i686 -fomit-frame-pointer -DDEBUG -ffast-math -frename-registers
DEBUG = -ggdb3 -DDEBUG -fno-inline 
PROF  = -O2 -march=i686 -DDEBUG -ggdb3 -fno-inline 

CFLAGS += -Wall #-static #-Wno-long-long -Wextra -Wno-variadic-macros

GPP    = g++ $(CFLAGS) 

all: ego_opt


ego_debug: board.cpp stack_board.cpp utils.cpp playout.cpp gtp.cpp gtp.h uct.cpp gtp_board.cpp main.cpp
	$(GPP) $(DEBUG) -o ego_debug gtp.cpp main.cpp

ego_opt:   board.cpp stack_board.cpp utils.cpp playout.cpp gtp.cpp gtp.h uct.cpp gtp_board.cpp main.cpp
	$(GPP) $(OPT)   -o ego_opt   gtp.cpp main.cpp

ego_prof:  board.cpp stack_board.cpp utils.cpp playout.cpp gtp.cpp gtp.h uct.cpp gtp_board.cpp main.cpp
	$(GPP) $(PROF)  -o ego_prof  gtp.cpp main.cpp



.SUFFIXES: .cpp .o

.cpp.o:
	$(GPP) -c $<

clean:
	rm -f *.s *.o .depend gmon.out core
	rm -f ego_debug ego_opt ego_prof
	rm -f *~
	rm -f *.orig
