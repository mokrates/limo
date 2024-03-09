#!/bin/bash

# apt-get install libgc-dev libgmp-dev libreadline-dev libssl-dev texinfo libsdl-dev libsdl-ttf2.0-dev libmhash-dev

# cd ..

make INSTALL_PREFIX=~/.local/ realclean
make INSTALL_PREFIX=~/.local/ TAGS
make INSTALL_PREFIX=~/.local/ limo-wsl
make INSTALL_PREFIX=~/.local/ install



