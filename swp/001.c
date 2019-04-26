#include <stdio.h>

#ifndef unix
#define unix 1
#endif

int
main()
{
	printf(&unix["\024%sworld\012"],-(24!='k')+unix[" hello "]+" hello "-0x66);
}
