Sherlock C++
============

C++ version of the `Sherlock <http://github.com/vmlaker/sherlock>`_
video processing codes using multi-threading
to accelerate performance on SMP (multi-core or multi-processor) systems.

Requirements
------------

In order to build the codes, you're gonna need
to have a few things available on your system:

1. `SCons <http://www.scons.org>`_ -- excellent Python-based build system
2. `OpenCV <http://www.opencv.org>`_ -- the *one-and-only* computer vision library
3. `Boost C++ libraries <http://www.boost.org>`_ -- if it ain't in the C++ standard, it's in Boost

With YUM package manager (Red Hat, CentOS, Fedora):
::
   
   yum install scons
   yum install opencv-devel
   yum install boost-devel

On systems using Aptitude package manager (Debian, Ubuntu):
::

   aptitude install scons
   aptitude install libopencv-dev
   aptitude install libboost-all-dev

Usage
-----

Download the *Sherlock C++* codes, and build them:
::

   git clone --recursive http://github.com/vmlaker/sherlock-cpp
   cd sherlock-cpp
   scons -j8 debug=0

Now, test the basic functionality. Try playing live video from
the first device (``/dev/video0``) for a duration of 10 seconds:
::

   bin/playcv 0 800 600 10

Motion detection
................

Motion detection is implemented with a run-of-the-mill
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

   bin/diffavg1 0 800 600 10

2. Now let's try the multi-threaded version utilizing shared memory.
Increased performance gained from parallel threads
will only be realized on SMP machines.
::

   bin/diffavg2 0 800 600 10

3. In case your display hardware does not keep up with the capture
and processing framerate, you may experience incremental lag
in playback of the previous case. The following case implements
lossy filtering of excess frames in the display pipeline, resulting in
a more "realtime" feel of the final output:
::

   bin/diffavg3 0 800 600 10

Object detection
................

We use Haar feature-based cascade classifiers to detect objects 
in the video stream. Active classifiers are listed in
``conf/classifiers.conf`` file -- by default, these are 
vanilla classifiers shipped with OpenCV distribution.

1. Run a sequential version of the algorithm:
::
   
   bin/detect1 0 800 600 10

2. Now try the parallel version, with individual classifiers running
in separate threads:
::
   
   bin/detect2 0 800 600 10



Cleanup
-------

To revert your repo to a clean state 
(like it was before the build) just do:
::

   scons -c
