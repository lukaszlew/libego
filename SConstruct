# Environments

env = Environment(
  CXX = "g++-4.2",
  CXXFLAGS = [
    "-DDEBUG",
    "-ggdb3",
    "-Wall",
    "-Wextra",
    "-Wswitch-enum",
  ],
)

opt = env.Clone()
opt.Append(CXXFLAGS = [
  "-O3",
  "-march=native",
  "-fomit-frame-pointer",
  "-ffast-math",
  "-frename-registers",
])

dbg = env.Clone()
dbg.Append(CXXFLAGS = [
  "-fno-inline",
])

SConscript('SConscript', build_dir='opt', exports={'env':opt})
SConscript('SConscript', build_dir='dbg', exports={'env':dbg})
