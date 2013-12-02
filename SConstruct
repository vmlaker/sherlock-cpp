"""The SCons file for Sherlock C++ codes."""

import os

# Retrieve the debug flag, if set.
debug = bool(int(ARGUMENTS.get('debug', False)))

# Build the Bites library.
SConscript('bites/SConstruct')

# Compile utils.
sources = (
    'src/util.cpp',
    'src/Captor.cpp',
    'src/Displayer.cpp',
    'src/Deallocator.cpp',
    'src/Detector.cpp',
)
libs = (
    'opencv_core',
#    'opencv_contrib',
    'bites',
)
env = Environment(
    CPPPATH=('include', 'bites/include'),
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
    'src/detect1.cpp',
    'src/detect2.cpp',
)
libs = (
    'boost_filesystem',
    'boost_thread',
    'boost_system',
    'opencv_core',
#    'opencv_contrib',
    'opencv_highgui',
    'opencv_imgproc',
    'opencv_objdetect',
    
    # Order is important: sherlock (1st) depends on coils (2nd).
    'sherlock',
    'bites',
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
