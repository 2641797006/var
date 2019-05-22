#include <stdio.h>

int dfa[][4] = 
{//	  .  '/' '*' '\n'
	{ 0,  1,  0,  0}, // 0 . /
	{ 0,  2,  3,  0}, // 1 / .
	{ 2,  2,  2,  0}, // 2 // \n
	{ 3,  3,  4,  3}, // 3 /* . *
	{ 3,  0,  4,  3}, // 4 . * */
};

//  test note
/*  test note  */

int sw(int);

int
main(int argc, char **argv)
{
	FILE *fin, *fout;
	int c, c1, state;

	if (argc < 3) {
		printf("argc < 3\n");
		return 0;
	}

	fin = fopen(argv[1], "r");
	if ( ! fin ) {
		printf("fail to open file `%s`\n", argv[1]);
		return 0;
	}

	fout = fopen(argv[2], "w");
	if ( ! fout ) {
		printf("fail to create file `%s`\n", argv[2]);
		return 0;
	}

	state = 0;
	for (;;) {
		c = fgetc(fin);
		if ( c == EOF )
			return 0;
		c1 = sw(c);
		c1 = dfa[state][c1];
		switch (c1) {
		case 0:
			if (state == 1)
				fputc('/', fout);
			if (state != 4)
				fputc(c, fout);
			break;
		}
		state = c1;
	}

	fclose(fin);
	fclose(fout);
}

int
sw(int c)
{
	switch (c) {
	case '/':
		return 1;
	case '*':
		return 2;
	case '\n':
		return 3;
	}
	return 0;
}
