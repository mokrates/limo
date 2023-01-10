DLLSOURCEDIR=/usr/i686-w64-mingw32/sys-root/mingw/bin
INSTALL_PREFIX=c:/limo/

if [ "$1" == "bin" ]; then
    make -f Makefile.mingw realclean
    #make TAGS
    make -f Makefile.mingw limo-mingw
    make -f Makefile.mingw install-win


    # TODO this here should be beefed up like this: https://blog.rubenwardy.com/2018/05/07/mingw-copy-dlls/
    for dll in $(i686-w64-mingw32-objdump.exe -p limo.exe | grep lib.*dll | awk '{print $3}'); do
	cp ${DLLSOURCEDIR}/$dll ${INSTALL_PREFIX}
    done
    cp ${DLLSOURCEDIR}/libgcc_s_sjlj-1.dll ${INSTALL_PREFIX}
    
else
    make -f Makefile.mingw install-mingw
fi

