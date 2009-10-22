Library of Effective GO routines
================================

This is a Library of Effective GO routines (libEGO), a high performance
implementation of Go rules and most popular algorithms used in computer Go.


Installation
------------

You will need:
 * git (version control system)
 * cmake (cross-platform, open-source build system)

Checkout the source:

    git clone git://github.com/lukaszlew/libego.git

Construct library and examples with your favorite compiler:

    export CXX=g++-4.2  # set your favorite compiler, optional
    ./cmakeall          # do all the work

Light playout benchmark:

    echo "benchmark" | ./bin/engine
    echo "benchmark 1000000" | ./bin/engine

Run exemplary GTP engine:

    ./bin/engine
    ./bin/engine config.gtp -

The second command will first execute GTP commands from config.gtp.
And then take commands from stdin.
You can connect engine to GoGui usigng both of above commands.

Thanks
------

My girlfriend for her endless support and patience.  
Benjamin Tauber for many hours of extreme programming.  
Jakub Pawlewicz for the idea of pseudo-liberties and find-union algorithms.  
Dave Hillis for the idea of mercy rule.  
Markus Enzenberger for his computer go bibliography and GoGui.  
Don Dailey for setting up CGOS server.  
And last but not least, a whole computer Go community for a
stimulating mixture of cooperation and competition.  

Copyright
---------

All files Copyright 2006 and onwards Łukasz Lew.

All files in this package are under General Public License,
which may be found in the file COPYING.

Contact
-------

Lukasz Lew <lukasz.lew@gmail.com>
