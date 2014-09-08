
Introduction
============

An opensource raytracer written in C++ with an embedded Scheme parser for scene files.

The sourcecode is released under the GPL. See the file COPYING.

Get hacking 
===========

The C++ source code is in `src`. Compile with

    ./autogen
    ./configure
    make
    sudo make install

The unit-tests are in `test`. Run them with

    make check

The `scenes` folder contains examples scenes. Render a scene with:

    raygay scenes/csg.scm out.png

Documentation written in Docbook is in `docs`. Build the HTML documentation with

    cd docs
    make chunked-portable




