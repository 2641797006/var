#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ResetCin()	while( fgetc(stdin) != '\n' ) {}

int main()
{
	char *a;
	int i, N, num, k;

	srand(time(NULL));

	printf("请输入随机数范围[1, N] N = ");
	scanf("%d", &N);
	ResetCin();
	N = N<1 ? 1 : N;
	a = malloc(N);
	memset(a, 0, N);

	printf("请输入随机数个数: ");
	scanf("%d", &num);
	ResetCin();
	num = num<1 ? 1 : num;
	num = num>N ? N : num;

	for (i=0; i<num; ++i) {
		k = rand()%N + 1;
		if ( ! a[k-1] ) {
			printf("%d ", k);
			a[k-1] = 1;
		}
		else
			--i;
	}
	puts("");
}
