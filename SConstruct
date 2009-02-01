# Basic environment

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

# Optimized target

opt = env.Clone()
opt.Append(CXXFLAGS = [
  "-O3",
  "-march=native",
  "-fomit-frame-pointer",
  "-ffast-math",
  "-frename-registers",
])

main_opt = opt.Object('ego_opt', 'main.cpp')
opt.Program(main_opt)

# Debug target

dbg = env.Clone()
dbg.Append(CXXFLAGS = [
  "-fno-inline",
])

main_dbg = dbg.Object('ego_dbg', 'main.cpp')
dbg.Program(main_dbg)
