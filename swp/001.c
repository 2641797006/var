#include <stdio.h>
#define true 1

int
main()
{
	printf(&true["\024%sworld\012"],-(24!='k')+true[" hello "]+" hello "-0x66);
}
