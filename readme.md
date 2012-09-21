smallcalc
---------
A small math calculation library. Compiles on Linux and iOS.

It can calculate with Fractions and provides some nice function printing, e.g:

    > 2+3/sin(0.5*PI)
             3           
    2 + ──────────── => 5
        sin(0.5 * π)


The library is used for µoscalc on iPad (http://itunes.apple.com/us/app/oscalc/id555689840)

Building
--------
Linux: You need cmake, gtest and a current version of boost. CMake shall find all dependencies and build the test application for you.

    # Clone it with git
    git clone git://github.com/nob13/smallcalc.git
    cd smallcalc
    # Create a build directory, go in there
    mkdir build; cd build
    # Let cmake generate make files
    cmake ../
    # build it
    make
    # Try the testcases
    testcases/testcases
    # Try the test app
    testapp/testapp

XCode (hack): Add boost to the include paths. Testcases are not supported. Just add the sourcefiles to your project.

I tried no other operating systems.

Credits
-------
Thanx to Nemanja Trifunovic for his Utf8 Library. I embedded his UTF8 routines in the 
printing package (smallcalc/print/utf8). http://utfcpp.sourceforge.net/

Thanx to the Guys at CERT for their overflow-safe mathematical routines:
https://www.securecoding.cert.org/confluence/x/RgE

Software Engineering in Berlin
------------------------------
If you have some C++ Linux / OSX / iOS stuff to be done in Berlin, tell me :) I am working as a Freelancer.

License
-------
The library, test application and testcases are licensed under the terms of the Apache License 2.0 license.

Copyright 2012 Norbert Schultz

