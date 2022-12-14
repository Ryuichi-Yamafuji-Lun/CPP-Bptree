#include "bptree.h"
#include <vector>
#include <sys/time.h>

struct timeval
cur_time(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t;
}

void
print_tree_core(NODE *n)
{
	printf("["); 
	for (int i = 0; i < n->nkey; i++) {
		if (!n->isLeaf) print_tree_core(n->chi[i]); 
		printf("%d", n->key[i]); 
		if (i != n->nkey-1 && n->isLeaf) putchar(' ');
	}
	if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
	printf("]");
}

void
print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n"); fflush(stdout);
}

void 
erase(NODE *node){
	int i;
	for (i = 0; i < N - 2; i++) {
		node->chi[i] = nullptr;
	}
	for (i = 0; i < N - 1; i++) {
		node->key[i] = 0; 
	}
	node->nkey = 0;
}


NODE *
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}

NODE *
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
	int i, j;
	//if key is less than the current lowest allocate the lowest spot to the key
	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		} 
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		//find where key fits in by placing key right above the value less than or equal to the key
		for (j = leaf->nkey; j > i; j--) {		
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		} 
		leaf->key[j] = key;
		leaf->chi[j] = (NODE *)data;

	}
	leaf->nkey++;

	return leaf;
}

TEMP *
sort_temp(TEMP *T, int key, DATA *data) 
{
	int i, j;
	if (key < T->key[0]) {
		for (i = T->nkey; i > 0; i--) {
			T->chi[i] = T->chi[i-1] ;
			T->key[i] = T->key[i-1] ;
		} 
		T->key[0] = key;
		T->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < T->nkey; i++) {
			if (key < T->key[i]) break;
		}
		//find where key fits in by placing key right above the value less than or equal to the key
		for (j = T->nkey; j >= i; j--) {		
			T->chi[j] = T->chi[j-1];
			T->key[j] = T->key[j-1];
		} 
		//place new key
		T->key[i] = key;
		T->chi[i] = (NODE *)data;
	}
	return T;
}

NODE *
alloc_leaf(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

TEMP *
alloc_temp(TEMP *L)
{
	TEMP *node;
	if (!(node = (TEMP *)calloc(1, sizeof(TEMP)))) ERR;
	node->isLeaf = true;
	node->nkey = 0;

	return node;
}

void 
insert_in_memory(TEMP *T, NODE *node)
{
	int i;
	for (i = 0; i < node->nkey; i++) {
		T->chi[i] = node->chi[i];
		T->key[i] = node->key[i];
	}
	T->chi[node->nkey] = node->chi[node->nkey];
	T->nkey = node->nkey;
}

void 
insert_in_parent(NODE *leaf, int key, NODE *L)
{
	//if leaf is the root of tree then make node R the root
	int i, j;
	if (leaf == Root) {
		//new node R
		NODE *R = alloc_leaf(NULL);
		//put leaf, key, L into R
		R->key[0] = key;
		R->chi[0] = leaf;
		R->chi[1] = L;
		leaf->parent = R;
		L->parent = R;
		R->nkey++;
		//make R the root
		Root = R;
		R->isLeaf = false;
		return;
	}
	//set P to N's parent
	leaf->parent = Root;
	//leaf split
	//if P has space then
	if (Root->nkey < N - 1) 
	{
		//insert new key and pointer to just after N
		if (L->key[0] < Root->key[0])
		{		
			for (i = Root->nkey; i > 0; i--) 
			{
				Root->key[i] = Root->key[i - 1];
				Root->chi[i + 1] = Root->chi[i];
			}
			Root->key[0] = L->key[0];
			Root->chi[1] = L;
		}
		else {
			for (i = 0; i <= Root->nkey; i++) {
				if (L->key[0] < Root->key[i]) break;
			}
			for (j = Root->nkey; j >= i; j--) {		
				Root->chi[j + 1] = Root->chi[j] ;
				Root->key[j] = Root->key[j-1] ;
			} 
			Root->key[j] = L->key[0];
			Root->chi[j + 1] = L;
		}
		Root->nkey++;

		return;
	}
	//non-leaf split
	else {
		int i, j;
		// Copy P to a block of memory T that hold P and (new key and pointer)
		TEMP *T = alloc_temp(NULL);
		insert_in_memory(T, Root);
		// Insert(new key and pointer) into T just after N
		if (L->key[0] < T->key[0]) 
		{
			for (i = T->nkey; i > 0; i--)
			{
				T->key[i] = T->key[i - 1];
				T->chi[i + 1] = T->chi[i];
			}
			T->key[0] = L->key[0];
			T->chi[1] = L;
		}
		else {
			for (i = 0; i <= T->nkey; i++) {
				if (L->key[0] < T->key[i]) break;
			}
			for (j = T->nkey; j >= i; j--) {		
				T->chi[j + 1] = T->chi[j] ;
				T->key[j] = T->key[j-1] ;
			} 
			T->key[j] = L->key[0];
			T->chi[j + 1] = L;
		}
		T->nkey++;
		// Erase all entries from P
		erase(Root);
		// Create node P'
		NODE *P = alloc_leaf(NULL);
		P->isLeaf = false;
		// Copy T.P1, . . . , T.P[(n+1)/2] into P
		int i_split = ceil((N + 1)/2);
		for (i = 0; i < i_split; i++) {
			Root->chi[i] = T->chi[i];
			Root->key[i] = T->key[i];
			Root->nkey++;
		}
		// Let K" = T.K[(n+1)/2]
		int K = T->key[i_split];
		int l = 0;
		// Copy T.P[(n+1)/2]+1, . . . , T.Pn+1 into P'
		for (j = i_split + 1; j <= N; j++) {
			P->chi[l] = T->chi[j];
			l++;
		}
		P->key[0] = T->key[i_split + 1];
		P->nkey++;
		// insert in parent(P, K", P')
		insert_in_parent(Root, K, P);
	}
		
}

void
set_ptr(NODE *leaf, NODE *L) {
	//Set leaf' ptr n to leaf ptr n
	L->chi[N - 1] = leaf->chi[N - 1];
	//set leaf ptr to leaf'
	leaf->chi[N - 1] = L;
	L->parent = leaf;
}

void 
insert(int key, DATA *data)
{
	NODE *leaf;
	//if not root node then leaf is root
	if (Root == NULL) {
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
	else {
    leaf = find_leaf(Root, key);
  	}
	//if space in leaf then insert
	if (leaf->nkey < (N-1)) {
		insert_in_leaf(leaf, key, data);
	}
	else { // SPLIT
		int i, j;
		//create node leaf'
		NODE *L = alloc_leaf(NULL);
		//Copy leaf key and data to a block of memory T that can hold N pairs
		TEMP *T = alloc_temp(NULL);

		insert_in_memory(T, leaf);
		//insert new key and pointer to data
		sort_temp(T, key, data);
		//Set ptr
		set_ptr(leaf, L);
		//erase leaf.ptr1 through leaf.keyn-1 from leaf
		erase(leaf);
		//copy T.data1 through T.data[n/2] from T into Leaf starting at leaf.data1
		for (i = 0; i < ceil(N/2); i++) {
			leaf->chi[i] = T->chi[i];
			leaf->key[i] = T->key[i];
			leaf->nkey++;
		}
		//copy T.data[n/2]+1 through T.keyn from T into leaf' starting at leaf'.data
		int k = 0;
		for (j = ceil(N/2); j < N; j++) {
			L->chi[k] = T->chi[j];
			L->key[k] = T->key[j];
			L->nkey++;
			k++;
		}
		//let key' be the smallest key in leaf'
		int _key = L->key[0];
		//insert in parent(leaf, key', leaf')
		insert_in_parent(leaf,_key,L);
	}
}

void
init_root(void)
{
	Root = NULL;
}

int 
interactive()
{
  int key;

  std::cout << "Key: ";
  std::cin >> key;

  return key;
}

int
main(int argc, char *argv[])
{
	struct timeval begin, end;
	int x = 0;
	init_root();
	begin = cur_time();
  	while (x < 100) {
		DATA *record = (DATA *)malloc(sizeof(DATA));
		record->val = 3;
		int rc = pthread_rwlock_init(&(record->rwlock), NULL);
		if(rc == -1) ERR;
		insert(x, record);	
		//print_tree(Root);
		x++;
  	}
	end = cur_time();
	printf(end);
	return 0;
}
