
	   _ _                \
      | (_)_ __ ___   ___  \  (λ)
      | | | '_ ` _ \ / _ \ |\~~|
      | | | | | | | | (_) || \ |
      |_|_|_| |_| |_|\___/ |___|

       
	   
written by Moritz Molle

Licensed under GPL (v3) (applies to all files in this tarball)
http://www.gnu.org/licenses/gpl.html

![CodeQL](https://github.com/mokrates/limo/actions/workflows/codeql.yml/badge.svg?branch=master "CodeQL status")

____

# for the impatient:

	$ bash build.sh bin
	
builds and installs limo into `~/.local/bin` and `~/.local/lib`

Installing systemwide:

	$ make 
	$ make install

builds and installs into /usr/local/

## Custom prefix
You can define a prefix where limo should be installed by giving make
a `INSTALL_PREFIX`:

	$ make clean
	$ make INSTALL_PREFIX=/usr/local/ limo
	$ make INSTALL_PREFIX=/usr/local/ install
	
please be aware that the `INSTALL_PREFIX` should be given to all the
make targets.

It it advisable to always make clean before every build. `build.sh` does this.

`build.sh`:  
build.sh is _really_ simple, I recommend to modify it to your needs and use that.


-------------------------------------------------------------------------------

# dependencies:

	build-essential (gcc, make)
	libgc (boehm garbage collector)
	libreadline
	libgmp (gnu multiprecision library)
	libpthread

really, see `deps/`, meanwhile there's a whole lot of new libraries
available

# Libraries:
You can choose to build libraries or not to build them:
If you choose not to build libraries, you have to remove them from
libs/Makefile

If you choose to build a library with limo you can choose to inline
libraries or to build them as shared objects. If you want to inline a
library, add it to inlined.mods

## Library dependencies:
ncurses: ncurses
ncmenu: ncurses ncmenu (if you inline ncmenu, you have also have to inline ncurses)
sdl: sdl
simplesdl: sdl
limogtk: gtk3

limogtk
=======
Not built by default. This is very dirty stuff.
1. You need a working limo installed to build limogtk
2. this takes a while. The scripts generate C code from the gtk header files.

~/limo$ cd libs/limogtk
~/limo/libs/limogtk$ make clean
~/limo/libs/limogtk$ make
~/limo/libs/limogtk$ cd ../..

   ~/limo$ make install
or ~/limo$ bash build.sh


Building on MAC OS X
====================
You can install all the dependencies using homebrew (http://brew.sh)
You have to use the limo-wsl or limo-almost-static target.

Building on Windows Subsystem for Linux (WSL)
=============================================
You have to use the limo-wsl or limo-almost-static target.

Attributions and Licenses
=========================
As not 100% of the code is mine, please have a look at attributions.txt

-------------------------------------------------------------------------------

specials:

	on REPL:    _	: the last evaluated value

	(try TRY CATCH)	: the TRY form is tried. if an error occurs, the CATCH form will be executed
	                  In the CATCH form, there is a var _EXCEPTION which contains the
					  argument to throw().

	_exception	: see try

	(eval EXP [ENV]): evaluate EXP under environment ENV. EXP and ENV are evaluated once under the
		              normal environment, as befits a function. THEN EXP is evaluated again.

	dcons/#<thunk>	: #<thunk> are instances of a type which cannot be instatiated by a program.
	  	  what CAN be instantiated are dcons'es.
		  #<thunk> is a type used to implement tailcall-optimization. if an EXP is the
		  last in an eval-chain, it is not evaluated, but packed into an #<thunk>
		  (evaluate again) and returned. this is purely internal.
		  it can be used with dcons(). dcons is a builtin, which evaluates it's first
		  argument, packs the second into an #<thunk> and cons'es those two.
		  this way the cdr of the cons only gets evaluated, when cdr() is called on the
		  cons. this is limo's way of implementing iterators.

There's tons of other stuff:
language:

- tailcall optimization
- multithreading
- objects/classes
- bignums/quotients
- macros

simple libraries:
- simplest networking (sockets)
- simple graphics (sdl/gtk/ncurses)

EXAMPLES
========

Tetris
------
You can play tetris
limo examples/tetris.limo

Snake
-----
You can see snake beat itself:
limo examples/autosnake2.limo 10 10

mandelbrot
----------
The usual mandelbrot fractal rendered with sdl

more
----
just see the examples/-directory.
