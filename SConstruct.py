"""The SCons file for Sherlock C++ codes."""

import os

# Build the Bites library.
SConscript('bites/SConstruct')

# Build the test program.
sources = (
    'src/playcv2.cpp',
)
libs = (
    'bites',
    'opencv_core',
    'opencv_contrib',
    'opencv_highgui',
)
env = Environment(
    CPPPATH='bites/include',
    LIBPATH='bites/lib',
    LIBS=libs,
    CXXFLAGS='-std=c++11',
) 
for source in sources:
    target = source[:source.rfind('.')]
    target = os.path.basename(target)
    target = os.path.join('bin', target)
    env.Program(target, source)
