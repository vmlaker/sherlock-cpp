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

Now run the codes. For starters, let's simply play live video from
the first device (``/dev/video0``) for a duration of 10 seconds:
::

   bin/playcv 0 640 480 10

Next, let's try motion detection, a run-of-the-mill
foreground/background segmentation algorithm using scene average.
Each iteration of ``diffavg`` code increases in complexity, 
with cumulative changes intended to enhance performance. 
You can compare processing performace by comparing framerates 
(displayed in upper left corner) for different cases.
Try running at high (at or near maximum) resolution of your camera. 
Also, consider profiling resource usage by running the codes
prefixed with the ``time`` command.

1. First, run the baseline case, a sequential implementation
of the algorithm:
::

   bin/diffavg1 0 640 480 10

2. Now let's try the multi-threaded version utilizing shared memory.
Increased performance gained from parallel threads
will only be realized on multi-core (or multi-processor) SMP machines.
::

   bin/diffavg2 0 640 480 10

3. In case your display hardware does not keep up with the capture
and processing framerate, you may experience incremental lag
in playback of the previous case. The following case implements
lossy filtering of excess frames in the display pipeline for 
a more "realtime" feel of the final output:
::

   bin/diffavg3 0 640 480 10


Cleanup
-------

To revert your repo to a clean state 
(like it was before the build) just do:
::

   scons -c
