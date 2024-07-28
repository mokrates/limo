if [ "$1" == "localbin" ]; then
    make INSTALL_PREFIX=~/.local/ realclean
    make INSTALL_PREFIX=~/.local/ TAGS
    make INSTALL_PREFIX=~/.local/ limo-wsl
    make INSTALL_PREFIX=~/.local/ info
    make INSTALL_PREFIX=~/.local/ install
    echo "limo should now be installed into ~/.local/ in your home directory"
elif [ "$1" == "systembin" ]; then
    make realclean
    make TAGS
    make limo-wsl
    make info
    echo "now please run 'sudo make install'"
elif [ "$1" == "local" ]; then
    make INSTALL_PREFIX=~/.local/ install
    echo "limo wasn't built but installed. limo-code should be updated in ~/.local/ in your home directory"
elif [ "$1" == "system" ]; then
    echo "this can't install anything. please run 'sudo make install'"
else
    echo "please run 'bash build.sh [ localbin | systembin | local | system ]"
fi
