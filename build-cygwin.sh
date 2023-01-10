if [ "$1" == "bin" ]; then
    make INSTALL_PREFIX=/usr/local/ realclean
    make INSTALL_PREFIX=/usr/local/ TAGS
    make INSTALL_PREFIX=/usr/local/ limo-cygwin
    make INSTALL_PREFIX=/usr/local/ install
else
    make INSTALL_PREFIX=/usr/local/ install
fi
