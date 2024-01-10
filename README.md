# MFVGT
Maintenance Files Versions Grouppers Tool</Br>

Pour toute aide sur le fonctionnment graphique : https://github.com/jeromefavrou/MFVGT/blob/main/help.pdf</Br>

ppour toute installation par un installeur : https://github.com/jeromefavrou/MFVGT/releases

pour compilation manuel</Br>

Pour WIndows installer MSYS2 https://www.msys2.org</Br>

puis </Br>

pour 32bit MSYS2 cmd a taper</Br>

pacman -Syuu</Br>
pacman -S mingw-w64-i686-toolchain</Br>
pacman -S mingw-w64-i686-gtkmm3</Br>
pacman -S mingw-w64-i686-pkg-config</Br>
pacman -S mingw-w64-i686-crypto++</Br>
pacman -S mingw-w64-i686-poppler</Br>
pacman -Syuu</Br>
i686-w64-mingw32-pkg-config --cflags --libs gtkmm-3.0 </Br>

pour 64bit MSYS2 cmd a taper</Br>

pacman -Syuu</Br>
pacman -S mingw-w64-x86_64-toolchain</Br>
pacman -S mingw-w64-x86_64-gtkmm3</Br>
pacman -S mingw-w64-x86_64-pkg-config</Br>
pacman -S mingw-w64-x86_64-crypto++</Br>
pacman -S mingw-w64-x86_64-poppler</Br>
pacman -Syuu</Br>
x86_64-w64-mingw32-pkg-config --cflags --libs gtkmm-3.0 </Br>
