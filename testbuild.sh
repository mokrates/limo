if [ "$1" == "bin" ]; then
    rm -rI testbuild
    mkdir testbuild
    make INSTALL_PREFIX=$PWD/testbuild/ realclean
    make INSTALL_PREFIX=$PWD/testbuild/ TAGS    
    make INSTALL_PREFIX=$PWD/testbuild/ limo-wsl
    make INSTALL_PREFIX=$PWD/testbuild/ install
else
    make INSTALL_PREFIX=$PWD/testbuild/ install
fi

