#include <stdio.h>
#include <string.h>
#include "string24k.h"

int main()
{
	int i, c, c1, max=0, index=0, sub_len=0, sub_index=0;
	char state[256], *pc;
	string _s, *s=&_s;

	memset(state, 0, 256);
	string_init(s);

	printf("请输入一个字符串: ");
	s->getline(s);

	pc = s->data(s);

	for (;;) {
		c = *pc++;
		if ( ! c )
			break;
		if ( ! state[c] ) {
			state[c] = 1;
			++sub_len;
		} else {
			if ( sub_len > max )
				max = sub_len, index = sub_index;
			for (;;) {
				c1 = s->at(s, sub_index++);
				if (c1 == c)
					break;
				--sub_len;
				state[c1] = 0;
			}
		}
	}

	if ( sub_len > max )
		max = sub_len, index = sub_index;

	printf("%s\nsub_index = %d, sub_len = %d\n", s->c_str(s), index+1, max);
	for (i=index; i<index+max; ++i)
		putchar(s->at(s, i));
	putchar('\n');

	return 24-'k';
}
