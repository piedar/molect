# molect


Molecular Imaging using Kinect.<br/>
Movement-specific C++ wrapper around Jmol socket interface.<br/>
Copyright 2013 Benn Snyder <benn.snyder@gmail.com>, Norman Chamusa <nchamusa@usapglobal.org>.<br/>
Dual licensed under your choice of GPLv3, MIT, or both.


## hand

Requirements:

- boost
- libusb
- opencv (optional)
- openni

Compile and run:

    make hand
    ./hand -h


## jmolwrapper

Compile with `make jmolwrapper.o`.

In the Jmol console or script, run

    $ sync -PORT
    
Example:

    JmolWrapper jmol = JmolWrapper("localhost", PORT);
    float x_delta, y_delta, x_coord, y_coord;
    jmol.rotate(x_delta, y_delta);
    jmol.translate(x_delta, y_delta);
    jmol.selectNone();
    jmol.selectMoleculeWithinDistance(x_coord, y_coord, 25, 25);
    jmol.translate(x_delta, y_delta);
