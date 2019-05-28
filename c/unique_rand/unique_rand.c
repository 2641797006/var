#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define _24k_btree_t int
#include "btree.h"

#define ResetCin()	while( fgetc(stdin) != '\n' ) {}

int main()
{
	int i, N, num, k;
	BTNode *root;

	srand(time(NULL));
	root = CreateBTN(NULL);

	printf("请输入随机数范围[1, N] N = ");
	scanf("%d", &N);
	ResetCin();
	N = N<1 ? 1 : N;

	printf("请输入随机数个数: ");
	scanf("%d", &num);
	ResetCin();
	num = num<1 ? 1 : num;
	num = num>N ? N : num;

	for (i=0; i<num; ++i) {
		k = rand()%N + 1;
		if ( BtreeInsert(root, &k) )
			printf("%d ", k);
		else
			--i;
	}
	puts("");
}
