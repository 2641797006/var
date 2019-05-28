#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define _24k_btree_t int
//#include "btree.h"

#ifndef _BTREE_H_
#define _BTREE_H_

#ifndef _24k_btree_t
# error: No define "_24k_btree_t"
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _MEMORY_H
#include <memory.h>
#endif

#define MAX_T	4
#define MIN_T	((MAX_T+1)/2)

typedef struct BTNode{
	int		keynum;
	struct BTNode	*parent;
	_24k_btree_t	key[MAX_T];
	struct BTNode	*child[MAX_T+1];
}BTNode, *BTNode_P;

BTNode*
CreateBTN(BTNode *parent)
{
	BTNode *btnode=malloc(sizeof(BTNode));
	if(!btnode)
		return NULL;
	btnode->keynum=0;
	btnode->parent=parent;
	return btnode;
}

int
BtreeInsert(BTNode *T, _24k_btree_t *key)
{
	int i, j, leaf;
	BTNode *T1, *Tp;
	for(;;){
		for(i=0;i<T->keynum;i++)
			if(*key<=T->key[i])
				break;
		if (i < T->keynum && *key==T->key[i])
			return 0;
		if(T->child[i])
			T=T->child[i];
		else
			break;
	}
	for(j=T->keynum;j>i;j--)
		T->key[j]=T->key[j-1];
	T->key[i]=*key;
	T->keynum++;
	leaf=1;
	while(T->keynum==MAX_T){
		T1=CreateBTN(T->parent);
		for(i=MIN_T;i<MAX_T;i++){
			T1->key[i-MIN_T]=T->key[i];
			if(!leaf){
				T->child[i]->parent=T1;
				T1->child[i-MIN_T]=T->child[i];
			}
		}
		if(!leaf){
			T->child[i]->parent=T1;
			T1->child[i-MIN_T]=T->child[i];
		}
		T1->keynum=MIN_T;
		T->keynum=MIN_T-1;
		Tp=T->parent;
		if(!Tp){
			Tp=T;
			T1->parent=Tp;
			T=CreateBTN(Tp);
			for(i=0;i<MIN_T-1;i++){
				T->key[i]=Tp->key[i];
				if(!leaf){
					Tp->child[i]->parent=T;
					T->child[i]=Tp->child[i];
				}
			}
			if(!leaf){
				Tp->child[i]->parent=T;
				T->child[i]=Tp->child[i];
			}
			T->keynum=MIN_T-1;
			Tp->key[0]=Tp->key[MIN_T-1];
			Tp->child[0]=T;
			Tp->child[1]=T1;
			Tp->keynum=1;
			break;
		}
		for(i=0;i<Tp->keynum;i++)
			if(Tp->child[i]==T)
				break;
		for(j=Tp->keynum;j>i;j--){
			Tp->key[j]=Tp->key[j-1];
			Tp->child[j+1]=Tp->child[j];
		}
		Tp->key[i]=T->key[MIN_T-1];
		Tp->child[i+1]=T1;
		Tp->keynum++;
		T=Tp;
		leaf=0;
	}
	return 1;
}

#endif


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
