if [ "$1" == "bin" ]; then
    make INSTALL_PREFIX=~/.local/ realclean
    make INSTALL_PREFIX=~/.local/ TAGS
    make INSTALL_PREFIX=~/.local/ limo-wsl
    make INSTALL_PREFIX=~/.local/ install
else
    make INSTALL_PREFIX=~/.local/ install
fi

