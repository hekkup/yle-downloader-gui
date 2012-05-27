# YLE Areena downloader GUI #

http://mpartel.github.com/yle-downloader-gui/

This is a GUI for [RTMPDump-YLE](http://users.tkk.fi/~aajanki/rtmpdump-yle/index.html), a modified rtmpdump program by Antti Ajanki for downloading clips from [YLE Areena](http://areena.yle.fi/).

The GUI is written in C++/[Qt](http://qt.nokia.com/) using [QtCreator](http://qt.nokia.com/products/developer-tools/) and currently requires rtmpdump-yle to be installed.

A precompiled Windows package including the downloader is available on the download page.

## Compiling ##

- `lrelease yle-downloader-gui.pro`
- `qmake`
- `make`

## Packaging for Windows ##

- Use the [build script](https://github.com/mpartel/rtmpdump-yle-windows) to compile rtmpdump-yle for Windows.
- Put the following in the distribution directory:
    - The directory `yle-dl-windows-x.y.z` compiled by the build script.
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
    - `yle-downloader-gui.exe`
- Make a zip package of the above.
- Use [Inno Setup](http://www.jrsoftware.org/isinfo.php) to compile `installer/innosetup_installer.iss`. You may need to edit the paths at the beginning of the script to point to where you gathered the above files.

## Known issues ##

- On windows, trying to download a file that already exists, yle-dl crashes to an encoding error while printing a log message.

## Thanks ##

- Antti Ajanki for yle-dl
- [Jesse Jaara](https://github.com/Huulivoide) for a better icon and suggestions.
- [Tomas Strand](https://github.com/straend) for YLE Passi support.

## Version history ##

- 2.0 (2012-05-28)
    * Updated yle-dl to 1.99.7.

- 1.99.1 (2012-04-24)
    * Updated yle-dl to 1.99.6.
    * Added subtitle selection box.

- 1.99 (2012-04-17)
    * Support for Areena Beta.

- 1.3 (2012-01-02)
    * Added support for YLE Passi.

- 1.2 (2011-12-05)
    * Updated rtmpdump-yle to 1.4.6, which fixed some Areena downloads.

- 1.1 (2011-10-11)
    * Updated rtmpdump-yle to 1.4.4, which fixed Elävä Arkisto and YleX on Windows.
    * Passed the `--vfat` switch on Windows.

## License ##

Public Domain, but note that yle-dl is under GPLv2, so treat this as GPL'ed too if you like. I don't really care :-)

