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

    ./cmakeall          # do all the work

Run GTP engine:

    ./bin/libego.sh     # Same as ./bin/engine but loads pattern gammas from a file.
    ./bin/libego.sh gui # Experimental gui for making experiments :)

You can connect engine to GoGui using first command.

Thanks
------

My girlfriend for her endless support and patience.  
Benjamin Tauber for many hours of extreme programming.  
Jakub Pawlewicz for the idea of pseudo-liberties and find-union algorithms.  
Dave Hillis for the idea of mercy rule.  
Markus Enzenberger for his computer go bibliography and GoGui.  
Don Dailey for setting up CGOS server.  
Remi Coulom for supervising my work.  
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
