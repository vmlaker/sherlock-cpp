"""The SCons file for Sherlock C++."""

# Build the test program.
sources = (
    'src/playcv2.cpp',
)
libs = (
    'opencv_core',
    'opencv_contrib',
    'opencv_highgui',
)
env = Environment(
    LIBS=libs,
    CXXFLAGS='-std=c++11',
) 
for source in sources:
    target = source[:source.rfind('.')]
    env.Program(target, source)
