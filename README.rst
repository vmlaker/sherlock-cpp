Sherlock C++
============

C++ version of the `Sherlock <http://github.com/vmlaker/sherlock>`_
video processing codes using multi-threading and shared memory
for high performance on SMP (multi-core or multi-processor) systems.

Requirements
------------

In order to build the codes, you're gonna need
to have a few things available on your system:

1. `SCons <http://www.scons.org>`_ -- excellent Python-based build system
2. `OpenCV <http://www.opencv.org>`_ -- the *one-and-only* image processing library
3. `Boost C++ libraries <http://www.boost.org>`_ -- if it ain't in the C++ standard, it's in Boost

With YUM package manager (Red Hat, CentOS, Fedora):
::
   
   yum install scons opencv-devel boost-devel

On systems using Aptitude package manager (Debian, Ubuntu):
::

   aptitude install scons libopencv-dev libboost-all-dev

Usage
-----

Download the *Sherlock C++* codes and
the auxiliary `Bites <http://vmlaker.github.io/bites>`_ tools library,
then build the software stack:
::

   git clone http://github.com/vmlaker/sherlock-cpp
   git clone http://github.com/vmlaker/bites
   cd sherlock-cpp
   scons bites=../bites

Test basic functionality by playing live video from
the first device (``/dev/video0``) for a duration of 10 seconds:
::

   bin/playcv 0 800 600 10

Object detection
................

Objects in the video stream are detected using Haar feature-based 
cascade classifiers. Active classifiers are listed in
``conf/classifiers.conf`` file. By default, these are 
vanilla classifiers shipped with OpenCV distribution.
You can edit this file to activate (or deactivate) classifiers,
change search paths, add your own custom classifiers,
and configure global object detection parameters.

The parallel algorithm distributes tasks among multiple
threads, a separate thread running one of the following tasks:

A) Allocation of frame memory and video capture.
B) Object detection (one thread per each Haar classifier.)
C) Augmenting output with detection result and displaying the frame.   
D) Memory deallocation.

.. image:: https://raw.github.com/vmlaker/sherlock-cpp/master/diagram.png

Memory for every captured frame is shared between all threads.
You can run the object detection algorithm with:
::
   
   bin/detect 0 800 600 10

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

Cleanup
-------

To revert your repo to a clean state 
(like it was before the build) just do:
::

   scons -c
