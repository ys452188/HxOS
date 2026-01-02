#ifndef HXOS_ICON_H
#define HXOS_ICON_H
#include "VGA.h"
#define HXOS_OS_ICON_ASCII \
"\n\
                \33[31m HXH       HXH\n\
                \33[31m HXH       HXH\33[0m    MMMMMM     AMMMMMA\n\
                \33[31m HXHHHHHHHHHXH\33[0m  AMV    VMA AM\n\
                \33[31m HXHXXXXXXXHXH\33[0m  MMM    MMM WM\n\
                \33[31m HXH \33[34mHA AH \33[31mHXH\33[0m  MMM    MMM  VMMMMMMA\n\
                \33[31m HXH   \33[34mX   \33[31mHXH\33[0m  VWA    AWV        VMM\n\
                \33[31m HXH \33[34mHV VH \33[31mHXH\33[0m    WWWWWW    VMMMMMMV\n\
\33[0m"
void displayHXOSIcon(void) {
	//setColor(0x04);   //黑底红字
	vga_write(HXOS_OS_ICON_ASCII);
	setColor(0x0F);
}
#endif