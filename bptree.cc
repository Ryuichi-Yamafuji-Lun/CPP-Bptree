#include "bptree.h"
#include <vector>
#include <sys/time.h>

#define num_threads 4
#define num_of_data 1000000

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
	//proceed to insert
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

DATA *
search_record(int key)
{
	NODE *leaf;
	int i;
	//find leaf
	leaf = find_leaf(Root, key);
	//find chi array corresponding leaf node
	for (i = 0; i < leaf->nkey - 1; i++) {
		if (key < leaf->key[i]) break;
	}
	//return record
	return (DATA *)leaf->chi[i];
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

void
back_off(DATA * record){
	sleep(rand() % 20 + 1);
	int rc = pthread_rwlock_init(&(record->rwlock), NULL);
	if (rc == -1) ERR;
}

//update through binary tree 
void* 
update_val(void* argp)
{	
	DATA *record;
	int value;
	int key = 1 + (rand() % num_of_data);
	//search for the location of the record
	//go into record
	record = search_record(key);
	//check if 
	int rc = pthread_rwlock_wrlock(&(record->rwlock));
	if(rc == -1){
		back_off(record);
	}
	// std::cout << "Value: ";
	// std::cin >> value;
	value = 7;
	record->val = value;
	pthread_rwlock_unlock(&(record->rwlock));
	return NULL;
}

//read record through binary tree
void 
read_only(){
	DATA *record;
	int key = 1 + (rand() % num_of_data);
	record = search_record(key);
	int rc = pthread_rwlock_rdlock(&(record->rwlock));
	if(rc == -1){
		back_off(record);
	}
	printf("Read value at key %d: %d\n", key, record->val);
	pthread_rwlock_unlock(&(record->rwlock));
}

//one thread scans through the entire record
void*
scan(void* argp){
	DATA *record;
	int key = 0;
	record = search_record(key);
	while (key <= *(int *)argp){
		int rc = pthread_rwlock_rdlock(&(record->rwlock));
		if(rc == -1){
			back_off(record);
		}
		printf("Scan value: %d at %d\n", record->val, key);
		pthread_rwlock_unlock(&(record->rwlock));
		record = record->next;
		key++;
	}

	return NULL;
	
}

//read or write random 
/*
void *
read_or_write(void *argp){
	int r_or_w = 1 + (rand() % num_of_data);
	
	if(r_or_w == 0){
		//read
		read_only();
	} else {
		//update
		update_val();
	}
	return NULL;
}
*/

int
main(int argc, char *argv[])
{
	struct timeval begin, end;

	int x = 0;
	init_root();
	begin = cur_time();
	pthread_t thread_id[num_threads];

	DATA *record, *head, *tail;
	head = NULL;
  	while (x <= num_of_data) {
		record = (DATA *)malloc(sizeof(DATA));
		record->val = x;
		int rc = pthread_rwlock_init(&(record->rwlock), NULL);
		if(rc == -1){
			back_off(record);
		} 
		if (head == NULL){
			head = record;
			tail = record;
		}
		else{
			tail->next = record;
			tail = record;
		}
		insert(x, record);	
		x++;
  	}

	int num_of_scans = 1000;

	//n threads for updates
	for(int i = 0; i < num_threads; i++) {
		if (i != 0){
			pthread_create(&thread_id[i], NULL, update_val, NULL);
		} else {
			pthread_create(&thread_id[i], NULL, scan, &num_of_scans);
		}
		
	}
	//wait till all thread finishes
	for(int j = 0; j < num_threads; j++) {
		pthread_join(thread_id[j], NULL);
	}
	end = cur_time();
	printf("Time: %d milliseconds\n", end.tv_usec - begin.tv_usec);
	return 0;
}
