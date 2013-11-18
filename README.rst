Sherlock C++
============

C++ version of the 
`Sherlock <http://github.com/vmlaker/sherlock>`_
high-performance video processing codes.

Requirements
------------

In order to build the codes, you're gonna need
to have a few things available on your system:

1. `SCons <http://www.scons.org>`_ -- excellent Python-based build system
2. `OpenCV <http://www.opencv.org>`_ -- the *one-and-only* computer vision library
3. `Boost C++ libraries <http://www.boost.org>`_ -- if it ain't in the C++ standard, it's in Boost

::
   
   yum install scons
   yum install opencv-devel
   yum install boost-devel

Usage
-----

Download the *Sherlock C++* codes, and build them:
::

   git clone --recursive http://github.com/vmlaker/sherlock-cpp
   cd sherlock-cpp
   scons -j8 debug=0

Now run the codes. For starters, try to play live video from
the first device (``/dev/video0``) for a duration of 5 seconds:
::

   bin/playcv 0 640 480 5

Next, let's try motion detection. Each iteration of ``diffavg`` 
code increases in complexity, with cumulative changes intended
to enhance performance. You can compare processing performace
by comparing framerates (displayed in upper left corner) 
for different cases.

Increased performance gained by utilizing multiple threads
will only be realized on multi-core (or multi-processor) SMP machines.
Try running at high (at or near maximum) resolution of your camera. 
Also, consider profiling resource usage by running the codes
prefixed with the ``time`` command.

1. Run sequentially:
::

   bin/diffavg1 0 640 480 5

2. Run in multi-threaded fashion:
::

   bin/diffavg2 0 640 480 5

Cleanup
-------

To revert your repo to a clean state (like before the build)
just do:
::

   scons -c
