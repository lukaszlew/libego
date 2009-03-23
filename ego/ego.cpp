/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cctype>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <map>
#include <list>
#include <stack>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

using namespace std;

#include "ego.h"

#include "utils.cpp"

#include "fast_timer.cpp"
#include "fast_random.cpp"

#include "player.cpp"
#include "color.cpp"
#include "vertex.cpp"
#include "move.cpp"

#include "hash.cpp"
#include "board.cpp"
#include "sgf.cpp"

#include "playout.cpp"
#include "benchmark.cpp"

#include "gtp.cpp"
#include "basic_gtp.cpp"
#include "sgf_gtp.cpp"
