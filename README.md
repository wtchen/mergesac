##mergesac
-------------
Merges multiple Seismic Analysis Code binary files.

**Installation**

Simply run ``make install`` to compile and install to ``/usr/local/bin`` (assumed to be on the PATH).

**Usage**

`mergesac FILE1.SAC [FILE_N.SAC]`

**Notes**

The SAC files should be passed in chronological order. i.e. ``mergesac earliestfile.sac secondearliestfile.sac [...] latestfile.sac``

This program uses POSIX system calls.
