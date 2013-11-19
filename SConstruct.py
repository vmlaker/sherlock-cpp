"""The SCons file for Sherlock C++ codes."""

import os

# Retrieve the debug flag, if set.
debug = bool(int(ARGUMENTS.get('debug', False)))

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
    CXXFLAGS='-std=c++11',
)
if debug: env.Append(CXXFLAGS = ' -g')
env.Library('lib/sherlock', source=sources)

# Build the programs.
sources = (
    'src/playcv.cpp',
    'src/diffavg1.cpp',
    'src/diffavg2.cpp',
    'src/diffavg3.cpp',
)
libs = (
    'boost_thread',
    'opencv_core',
#    'opencv_contrib',
    'opencv_highgui',
    'opencv_imgproc',
    'bites',
    'sherlock',
)
env = Environment(
    CPPPATH=('bites/include', 'include'),
    LIBPATH=('bites/lib', 'lib'),
    LIBS=libs,
    CXXFLAGS='-std=c++11',
) 
if debug: env.Append(CXXFLAGS = ' -g')
for source in sources:
    target = source[:source.rfind('.')]
    target = os.path.basename(target)
    target = os.path.join('bin', target)
    env.Program(target, source)
