Sherlock C++
============

C++ version of the 
`Sherlock <http://github.com/vmlaker/sherlock>`_
video processing codes.

Requirements
------------

In order to build the codes, you're gonna need
to have a few things available on your system:

1. `SCons <http://www.scons.org>`_ -- excellent Python-based build system
2. `OpenCV <http://www.opencv.org>`_ -- the one-and-only computer vision library
3. `Boost C++ libraries <http://www.boost.org>`_ -- if it ain't in the C++ standard, it's in Boost

::
   
   yum install scons
   yum install opencv-devel
   yum install boost-devel

Download
--------

Get the *Sherlock C++* codes:
::

   git clone --recursive http://github.com/vmlaker/sherlock-cpp
   cd sherlock-cpp

Usage
-----

Build the codes:
::
   
   scons -j8 debug=1

Run the codes:
::

   bin/playcv2 -1 640 480 5
   bin/diffavg1 -1 640 480 5

Clean up after the build:
::

   scons -c
