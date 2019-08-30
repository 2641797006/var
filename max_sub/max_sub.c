#include <stdio.h>
#include <string.h>

int main()
{
	int i, c, c1, max=0, index=0, sub_len=0, sub_index=0;
	char s[4096], state[256], *pc;

	memset(state, 0, 256);

	printf("请输入一个字符串: ");
	scanf("%4000s", s);

	pc = s;

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
				c1 = s[sub_index++];
				if (c1 == c)
					break;
				--sub_len;
				state[c1] = 0;
			}
		}
	}

	if ( sub_len > max )
		max = sub_len, index = sub_index;

	printf("%s\nsub_index = %d, sub_len = %d\n", s, index+1, max);
	for (i=index; i<index+max; ++i)
		putchar(s[i]);
	putchar('\n');

	return 24-'k';
}
