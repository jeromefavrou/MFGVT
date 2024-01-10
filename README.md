# MFVGT\n
Maintenance Files Versions Grouppers Tool\n

pour compilation manuel\n

Pour WIndows installer MSYS2 https://www.msys2.org\n

puis \n

pour 32bit MSYS2 cmd a taper\n

pacman -Syuu\n
pacman -S mingw-w64-i686-toolchain\n
pacman -S mingw-w64-i686-gtkmm3\n
pacman -S mingw-w64-i686-pkg-config\n
pacman -S mingw-w64-i686-crypto++\n
pacman -S mingw-w64-i686-poppler\n
pacman -Syuu\n
i686-w64-mingw32-pkg-config --cflags --libs gtkmm-3.0 \n

pour 64bit MSYS2 cmd a taper\n
pacman -Syuu\n
pacman -S mingw-w64-x86_64-toolchain\n
pacman -S mingw-w64-x86_64-gtkmm3\n
pacman -S mingw-w64-x86_64-pkg-config\n
pacman -S mingw-w64-x86_64-crypto++\n
pacman -S mingw-w64-x86_64-poppler\n
pacman -Syuu\n
x86_64-w64-mingw32-pkg-config --cflags --libs gtkmm-3.0 \n
