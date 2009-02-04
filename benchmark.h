#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

namespace Benchmark {
  void run (Board const * start_board, 
            uint playout_cnt, 
            ostream& out, 
            bool score_per_vertex);
}

#endif
