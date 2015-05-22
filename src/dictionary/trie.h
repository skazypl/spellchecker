##ifndef TRIE_H
#define TRIE_H
	
#define MAX_SONS 26
#define LENG(x) (sizeof(x) / sizeof(x[0])) //sprawdzac tylko dla niepustej!

	struct Node
	{
		//int ifWord; //1 - slowo; 0 - nie
		//lepsza konwencja: pusty wezel tzn '\0'

		char* string;
		struct Node* parent;
		struct Node* children[MAX_SONS]; // wolalbym sons
		//todo: synowie alfabetycznie
		size_t childCount;
	};

	struct Tree
	{
		Node* root;
	};

	void Tree_init(struct Tree *t);
	void Tree_destroy(struct Tree *t);

	void add(struct Tree *t, char* word);
	int find(struct Tree *t, char* word); //1 - slowo jest w slowniku; 0 - nie ma

	size_t sizeOf(struct Tree *t);

	char* getWords(struct Tree *t);




#endif