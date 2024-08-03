/* A trie (pronounced as "try") or prefix tree is a tree data structure used to efficiently store and retrieve keys in a dataset of strings. There are various applications of this data structure, such as autocomplete and spellchecker.

Implement the Trie class:

Trie() Initializes the trie object.
void insert(String word) Inserts the string word into the trie.
boolean search(String word) Returns true if the string word is in the trie (i.e., was inserted before), and false otherwise.
boolean startsWith(String prefix) Returns true if there is a previously inserted string word that has the prefix prefix, and false otherwise.
 

Example 1:

Input
["Trie", "insert", "search", "search", "startsWith", "insert", "search"]
[[], ["apple"], ["apple"], ["app"], ["app"], ["app"], ["app"]]
Output
[null, null, true, false, true, null, true]

Explanation
Trie trie = new Trie();
trie.insert("apple");
trie.search("apple");   // return True
trie.search("app");     // return False
trie.startsWith("app"); // return True
trie.insert("app");
trie.search("app");     // return True */

#include "btree.h"
#include "hashtable.h"
#include "llist.h"
#include <string.h>
#include <strings.h>

typedef struct {
    TreeNode         *root;
    struct hashtable *ht;
} PrefixTree;

int hash(void *str, int a, int b)
{
    int hash = 5381;
    int c;

    while ((c = *(char*)str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

void trieInsert(struct hashtable *ht, TreeNode **t, void **a, int index, int n, int(hash)(void*, int, int))
{
    if(hash == NULL)
        hash = default_hashf;
    
    int key = hash(a[index], sizeof(a[index]), n);
    
    insertBtree2(*t, key, a);
    hashtable_put_bin(ht, &(key), sizeof(key), a[index]);
    
    trieInsert(ht, &(*t)->left, a, 2 * index + 1, n, hash);
    trieInsert(ht, &(*t)->right, a, 2 * index + 2, n, hash);
}

void *trieStarts(struct hashtable *ht, void *key, int key_size)
{
	int index = ht->hashf(key, key_size, ht->size);

	struct llist *llist = ht->bucket[index];

	struct htent cmpent;
	cmpent.key	= key;
	cmpent.key_size = key_size;

	struct htent *n = llist_find(llist, &cmpent, strstr());

	if (n == NULL) {
		return NULL;
	}

	return n->data;
}


int main()
{
    char *list[] = {"Trie", "insert", "dog", "search", "dog", "search", "do", "startsWith", "do", "insert", "do", "search", "do"};
    PrefixTree *pt;
    int length = 0;
    for (int i = 0; list[i] != NULL; i++) {
        length += 1;
    }
    
    pt->ht = hashtable_create(length, hash);
    trieInsert(pt->ht, &(pt->root), (void *)list, 0, length, hash);
}
