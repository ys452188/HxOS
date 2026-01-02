#ifndef HXOS_ICON_H
#define HXOS_ICON_H
#include "VGA.h"
#define HXOS_OS_ICON_ASCII \
"\n\
                HXH       HXH\n\
                HXH       HXH\n\
                HXHHHHHHHHHXH\n\
                HXHXXXXXXXHXH\n\
                HXH HA AH HXH\n\
                HXH   X   HXH\n\
                HXH HV VH HXH\n\
"
void displayHXOSIcon(void) {
	setColor(0x04);   //黑底红字
	vga_write(HXOS_OS_ICON_ASCII);
	setColor(0x0F);
}
#endif