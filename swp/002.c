#include <stdio.h>

int
main()
{
	long l[4]={ 0x206f6c6c656820L, 0x646c726f77732524, 0x000a, 0 };
	printf(&(-~0)[(char*)(l+1)],(-~0)[(char*)l]+(char*)l- -~0x66);
}
