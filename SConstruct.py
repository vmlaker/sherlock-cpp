"""The SCons file for Sherlock C++ codes."""

import os

# Build the Bites library.
SConscript('bites/SConstruct')

# Compile utils.
sources = (
    'src/util.cpp',
)
libs = (
    'opencv_core',
#    'opencv_contrib',
)
env = Environment(
    LIBS=libs,
    CXXFLAGS='-std=c++11', # -g',
) 
env.Library('lib/sherlock', source=sources)

# Build the programs.
sources = (
    'src/playcv2.cpp',
)
libs = (
    'opencv_core',
#    'opencv_contrib',
    'opencv_highgui',
    'bites',
    'sherlock',
)
env = Environment(
    CPPPATH=('bites/include', 'include'),
    LIBPATH=('bites/lib', 'lib'),
    LIBS=libs,
    CXXFLAGS='-std=c++11', # -g',
) 
for source in sources:
    target = source[:source.rfind('.')]
    target = os.path.basename(target)
    target = os.path.join('bin', target)
    env.Program(target, source)
