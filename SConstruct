# Environments

env = {}

env["base"] = Environment(
  CXX = "g++-4.2",
  CXXFLAGS = [
    "-DDEBUG",
    "-ggdb3",
    "-Wall",
    "-Wextra",
    "-Wswitch-enum",
  ],
  CPPPATH='#/ego'
)

env["opt"] = env["base"].Clone()
env["opt"].Append(
  CXXFLAGS = [
    "-O3",
    "-march=native",
    "-fomit-frame-pointer",
    "-ffast-math",
    "-frename-registers",
  ],
  LIBPATH = ['#/build/ego/opt']
)

env["dbg"] = env["base"].Clone()
env["dbg"].Append(
  CXXFLAGS = [
    "-fno-inline",
  ],
  LIBPATH = ['#/build/ego/dbg']
)

def build(subdir, variant):
  SConscript(
    subdir + '/SConscript',
    build_dir = 'build/' + subdir + '/' + variant,
    exports = {'env': env[variant]},
    duplicate = 0
  )

build('ego', 'dbg')
build('example', 'dbg')

build('ego', 'opt')
build('example', 'opt')
