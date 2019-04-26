#include <stdio.h>

int
main()
{
	printf(&(-~0)["\024%sworld\012"],(-~0)[" hello "]+" hello "- -~0x66);
}
