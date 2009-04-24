# Environments

env = {}

env["base"] = Environment(
  CXX = "g++-4.2",
  CXXFLAGS = [
    "-ggdb3",
    "-Wall",
    "-Wextra",
    "-Wswitch-enum",
    "-Wunused",
  ],
  CPPPATH='#/ego'
)

env["release"] = env["base"].Clone()
env["release"].Append(
  CXXFLAGS = [
    "-O3",
    "-march=native",
    "-fomit-frame-pointer",
    "-ffast-math",
    "-frename-registers",
  ],
  LIBPATH = ['#/build/ego/release']
)

env["debug"] = env["base"].Clone()
env["debug"].Append(
  CXXFLAGS = [
    "-fno-inline",
  ],
  LIBPATH = ['#/build/ego/debug']
)

env["testing"] = env["debug"].Clone()
env["testing"].Append(
  CXXFLAGS = [
    "-DTESTING"
  ],
  LIBPATH = ['#/build/ego/testing']
)

def build(subdir, variant):
  SConscript(
    subdir + '/SConscript',
    build_dir = 'build/' + subdir + '/' + variant,
    exports = {'env': env[variant]},
    duplicate = 0
  )

build('ego', 'debug')
build('ego', 'release')
build('ego', 'testing')

build('example', 'debug')
build('example', 'release')
build('example', 'testing')
