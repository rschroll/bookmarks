Browser Bookmarks Scope
=======================
View and open your bookmarks from the Dash.

Building
--------
The scope is designed to be built out-of-tree with CMake.  After downloading the
source and entering its directory, you'll want to do something along the lines
of
 ```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
If you open the project in Qt Creator, everything should just work.

The header files from `libsqlite3-dev` package are required to build the scope.
If your build is failing when tryingto find sqlite3.h, run `click-chroot-agent -r`
on your machine (directly; *not* inside the chroot).

License
-------
The Browser Bookmarks Scope is copyright 2015 Robert Schroll, and it is released
under the GPL v3 or later.  See the file LICENSE for the full terms.
