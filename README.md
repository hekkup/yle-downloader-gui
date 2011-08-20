# YLE Areena downloader GUI #

http://mpartel.github.com/yle-downloader-gui/

This is a GUI for [RTMPDump-YLE](http://users.tkk.fi/~aajanki/rtmpdump-yle/index-en.html), a modified rtmpdump program by Antti Ajanki for downloading clips from [YLE Areena](http://areena.yle.fi/).

The GUI is written in C++/[Qt](http://qt.nokia.com/) using [QtCreator](http://qt.nokia.com/products/developer-tools/) and currently requires rtmpdump-yle to be installed.

A precompiled Windows package including the downloader is available on the download page.


## Compiling ##

- `lrelease yle-downloader-gui.pro`
- `qmake`
- `make`

## Packaging for Windows ##

- Use the [build script](https://github.com/mpartel/rtmpdump-yle-windows) to compile rtmpdump-yle for Windows.
- Put the following in the distribution directory:
    - The directory `rtmpdump-yle-windows-x.y.z` compiled by the build script.
    - DLLs from the MinGW shipping with the Qt SDK
        - `libgcc_s_dw2-1.dll`
        - `mingwm10.dll`
        - `libeay32.dll`
        - `ssleay32.dll`
        - `QtCore4.dll`
        - `QtGui4.dll`
        - `QtNetwork4.dll`
        - `QtXml4.dll`
    - `README.md`
    - `Tietoa ohjelmasta.txt`
    - `yle-downloader-gui.exe`, obviously :)
- Make a zip package of the above.
- Use [Inno Setup](http://www.jrsoftware.org/isinfo.php) to compile `installer/innosetup_installer.iss`. You may need to edit the paths at the beginning of the script to point to where you gathered the above files.

## Bugs ##

- On Windows, the downloader makes file names with utf-8 umlauts.
- Any bugs the downloader may have.

## Thanks ##

- Antti Ajanki for yle-dl
- [Jesse Jaara](https://github.com/Huulivoide) for a better icon and suggestions.

## License ##

Public Domain, but note that yle-dl is under GPLv2, so treat this as GPL'ed too if you like. I don't really care :-)

