/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * This is GNU Go, a Go program. Contact gnugo@gnu.org, or see   *
 * http://www.gnu.org/software/gnugo/ for more information.      *
 *                                                               *
 * To facilitate development of the Go Text Protocol, the two    *
 * files gtp.c and gtp.h are licensed under less restrictive     *
 * terms than the rest of GNU Go.                                *
 *                                                               *
 * Copyright 2001, 2002, 2003, 2004, 2005 and 2006               *
 * by the Free Software Foundation.                              *
 *                                                               *
 * Permission is hereby granted, free of charge, to any person   *
 * obtaining a copy of this file gtp.h, to deal in the Software  *
 * without restriction, including without limitation the rights  *
 * to use, copy, modify, merge, publish, distribute, and/or      *
 * sell copies of the Software, and to permit persons to whom    *
 * the Software is furnished to do so, provided that the above   *
 * copyright notice(s) and this permission notice appear in all  *
 * copies of the Software and that both the above copyright      *
 * notice(s) and this permission notice appear in supporting     *
 * documentation.                                                *
 *                                                               *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY     *
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE    *
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR       *
 * PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO      *
 * EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS  *
 * NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR    *
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING    *
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF    *
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT    *
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS       *
 * SOFTWARE.                                                     *
 *                                                               *
 * Except as contained in this notice, the name of a copyright   *
 * holder shall not be used in advertising or otherwise to       *
 * promote the sale, use or other dealings in this Software      *
 * without prior written authorization of the copyright holder.  *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdarg.h>
#include <stdio.h>

/* Maximum allowed line length in GTP. */
#define GTP_BUFSIZE 1000

/* Status returned from callback functions. */
#define GTP_QUIT    -1
#define GTP_OK       0
#define GTP_FATAL    1

/* Whether the GTP command was successful. */
#define GTP_SUCCESS  0
#define GTP_FAILURE  1

/* Function pointer for callback functions. */
typedef int (*gtp_fn_ptr)(char *s);

/* Function pointer for vertex transform functions. */
typedef void (*gtp_transform_ptr)(int ai, int aj, int *bi, int *bj);

/* Elements in the array of commands required by gtp_main_loop. */
struct gtp_command {
  const char *name;
  gtp_fn_ptr function;
};

void gtp_main_loop(struct gtp_command commands[],
		   FILE *gtp_input, FILE *gtp_output, FILE *gtp_dump_commands);
void gtp_internal_set_boardsize(int size);
void gtp_set_vertex_transform_hooks(gtp_transform_ptr in,
				    gtp_transform_ptr out);
void gtp_mprintf(const char *format, ...);
void gtp_printf(const char *format, ...);
void gtp_start_response(int status);
int gtp_finish_response(void);
int gtp_success(const char *format, ...);
int gtp_failure(const char *format, ...);
void gtp_panic(void);
int gtp_decode_color(char *s, int *color);
int gtp_decode_coord(char *s, int *m, int *n);
int gtp_decode_move(char *s, int *color, int *i, int *j);
void gtp_print_vertices(int n, int movei[], int movej[]);
void gtp_print_vertex(int i, int j);
void gtp_append_commands (gtp_command* dst, gtp_command* src);

// helpful macros

#define decode_float(s, f) {                               \
  uint n;                                                  \
  if (sscanf (s, "%f%n", &f, &n) < 1)                      \
    return gtp_failure("syntax error");                    \
  s += n;                                                  \
}

#define decode_int(s, i) {                                 \
  uint n;                                                  \
  if (sscanf (s, "%d%n", &i, &n) < 1)                      \
    return gtp_failure("syntax error");                    \
  s += n;                                                  \
}

#define decode_str(s, str) {                               \
  uint n;                                                  \
  if (sscanf (s, "%s%n", str, &n) < 1)                     \
    return gtp_failure("syntax error");                    \
  s += n;                                                  \
}

#define decode_player(s, pl) {                             \
  int       gtp_color;                                     \
  uint n;                                                  \
                                                           \
  n = gtp_decode_color(s, &gtp_color);                     \
  if (n == 0) return gtp_failure("syntax error");          \
  s += n;                                                  \
                                                           \
  pl = (player::t) (2 - gtp_color);                        \
}

#define decode_player_v(s, plpl, vv, fail_i) {             \
  int       gtp_color;                                     \
  coord::t  r;                                             \
  coord::t  c;                                             \
  uint n;                                                  \
                                                           \
  n = gtp_decode_move(s, &gtp_color, &r, &c);              \
  if (n == 0) fail_i;                                      \
  else {                                                   \
    s += n;                                                \
                                                           \
    plpl    = (player::t) (2 - gtp_color);                 \
    vv      = vertex_t (r, c);                             \
  }                                                        \
}

#define decode_move(s, mm, fail_i) {                       \
  player::t pl;                                            \
  vertex_t v;                                              \
  decode_player_v(s, pl, v, fail_i);                       \
  mm = move::of_pl_v (pl, v);                              \
}

extern FILE *gtp_output_file;

/*
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 2
 * End:
 */
