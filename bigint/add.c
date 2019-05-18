#include <stdio.h>
#include <ctype.h>
#include <string.h>

int dfa[][3] =
{//	space 0  1~9
	{ 0,  1,  2}, // 0 space 0/1~9
	{-1,  1,  2}, // 1 0 space/1~9
	{-1,  2,  2}  // 2 0~9 space
};

int sw(int);
void get_num(char*);
char* str_rev(char*);

int
main()
{
	int i, ca, cb, carry=0;
	char a[256], b[256];

	get_num(a);
	get_num(b);

	str_rev(a);
	str_rev(b);

	for (i=0; ; ++i) {
		ca = a[i];
		cb = b[i];
		if ( ! ca && ! cb )
			break;
		if ( ! ca )
			ca = '0', a[i+1] = 0;
		if ( ! cb )
			cb = '0', b[i+1] = 0;
		ca += cb - '0' + carry;
		carry = 0;
		if ( ca > '9' )
			ca -= 10, carry = 1;
		a[i] = ca;
	}

	if ( carry )
		a[i] = '1', a[i+1] = 0;

	printf("%s\n", str_rev(a));
}

char* str_rev(char *s)
{
	int i, j, c;

	i = 0;
	j = strlen(s) - 1;
	while ( i < j ) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
		++i, --j;
	}
	return s;
}

void get_num(char *s)
{
	int i, c, c1, state;

	i = 0;
	state = 0;
	for (;;) {
		c = fgetc(stdin);
		c1 = sw(c);
//		if ( c1 < 0 )
//			break;
		c1 = dfa[state][c1];
		if ( c1 < 0 )
			break;
#if 0
		switch (c1) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			s[i++] = c;
			break;
		}
#else
		if ( c1 == 2 )
			s[i++] = c;
#endif
		state = c1;
	}
	if ( i == 0 )
		s[i++] = '0';
	s[i] = 0;
}

int sw(int c)
{
	if ( c == '0' )
		return 1;
	if ( isdigit(c) )
		return 2;
	if ( isspace(c) )
		return 0;
	return -1;
}

