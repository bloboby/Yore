#Yore

Yore is an under-developement roguelike console game.

##Build instructions:

At the moment Yore is capable of running only on Windows and Linux systems.

####To build for Linux:

1.  you must have make and gcc installed;
2.  [optional:] get rid of the -ggdb flags in makefile;
3.  run <pre>bash build-linux</pre>

####For Windows:

1.  you must have MinGW installed;
2.  run <pre>PATH &lt;your_mingw_install_path&gt;\bin<br />makefile.bat</pre>


###Yore runs (tested) on the following platforms:

 *  Ubuntu 11.10
 *  Windows XP
 *  Windows 7

Feel free to add to the list if you have a different operating system handy.

##Gameplay

Gameplay in Yore is largely similar to NetHack. 

###Movement

Movement with the hjkl set (yubn for diagonals) is supported; running using a capital letter (HJKLYUBN) is not implemented yet.
On windows you may also use the arrow keys to move, but you win't be able to move diagonally. Also, don't try moving through typed commands with the arrow keys - on Windows they have been mapped to hjkl and on Linux to ABCD.

###Miscellaneous

Various other commands are (in no particular order):
<pre>e      eat
,      pick up</pre>
