"""The SCons file for Sherlock C++ codes."""

import os

# Retrieve the debug flag, if set.
debug = bool(int(ARGUMENTS.get('debug', False)))

# Retrieve the Bites installation path.
bites_path = ARGUMENTS.get('bites', None)
if not bites_path:
    print('Please specify path to Bites installation, e.g. "bites=../bites"')
    exit(1)

# Build the Bites library (if not already done.)
SConscript(os.path.join(bites_path, 'SConstruct'))

# Assemble Bites include and library paths.
bites_inc_path = os.path.join(bites_path, 'include')
bites_lib_path = os.path.join(bites_path, 'lib')

# Assemble environment for building the library.
sources = (
    'src/util.cpp',
    'src/Captor.cpp',
    'src/Displayer.cpp',
    'src/Deallocator.cpp',
    'src/Classifier.cpp',
    'src/Detector.cpp',
)
libs = (
    'opencv_core',
#    'opencv_contrib',
    'bites',
)
env = Environment(
    CPPPATH=('include', bites_inc_path),
    LIBS=libs,
    CXXFLAGS='-std=c++11',
)
if debug: env.Append(CXXFLAGS = ' -g')

# Build the library.
lib = env.Library('lib/sherlock', source=sources)
Default(lib)  # Library is built by default.

# Assemble environment for building the programs.
sources = (
    'src/playcv.cpp',
    'src/diffavg1.cpp',
    'src/diffavg2.cpp',
    'src/diffavg3.cpp',
    'src/detect.cpp',
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
    CPPPATH=(bites_inc_path, 'include'),
    LIBPATH=(bites_lib_path, 'lib'),
    LIBS=libs,
    CXXFLAGS='-std=c++11',
) 
if debug: env.Append(CXXFLAGS = ' -g')

# Build the programs.
for source in sources:
    target = source[:source.rfind('.')]
    target = os.path.basename(target)
    target = os.path.join('bin', target)
    prog = env.Program(target, source)
    Default(prog)  # Program is built by default.

# Custom builder for running Doxygen.
doxy = Builder(
    action='doxygen $SOURCE',
)
env = Environment(BUILDERS={'Doxygen' : doxy})
env.Doxygen(target='doxygen', source='doxy.conf')
