#include <stdio.h>

int
main()
{
	long l=0x206f6c6c656820L;
	printf(&(-~0)["\024%sworld\012"],(-~0)[(char*)&l]+(char*)&l- -~0x66);
}
