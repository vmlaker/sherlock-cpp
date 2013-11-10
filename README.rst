Sherlock C++
============

C++ version of the 
`Sherlock <http://github.com/vmlaker/sherlock>`_
video processing codes.

Requirements
------------

In order to build the codes, you're gonna need 
`SCons <http://www.scons.org>`_
installed on your system:
::
   
   yum install scons

Download
--------

Get the *Sherlock C++* codes:
::

   git clone --recursive http://github.com/vmlaker/sherlock-cpp

Usage
-----

Build the codes:
::
   
   scons -j8

Run the simple playback example:
::

   bin/playcv2 -1 640 480 5

Clean up after the build:
::

   scons -c
