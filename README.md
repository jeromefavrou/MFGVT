# MFVGT
Maintenance Files Versions Grouppers Tool

pour compilation manuel

Pour WIndows installer MSYS2 https://www.msys2.org

puis 

pour 32bit MSYS2 cmd a taper

pacman -Syuu
pacman -S mingw-w64-i686-toolchain
pacman -S mingw-w64-i686-gtkmm3
pacman -S mingw-w64-i686-pkg-config
pacman -S mingw-w64-i686-crypto++
pacman -S mingw-w64-i686-poppler
pacman -Syuu
i686-w64-mingw32-pkg-config --cflags --libs gtkmm-3.0 

pour 64bit MSYS2 cmd a taper
pacman -Syuu
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-gtkmm3
pacman -S mingw-w64-x86_64-pkg-config
pacman -S mingw-w64-x86_64-crypto++
pacman -S mingw-w64-x86_64-poppler
pacman -Syuu
x86_64-w64-mingw32-pkg-config --cflags --libs gtkmm-3.0 
