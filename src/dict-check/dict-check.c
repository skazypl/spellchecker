#include <stdio.h>
#include <stdlib.h>
#include <dictionary.h>
#include <string.h>
#include <locale.h>
#include <wctype.h>
#include <errno.h>

const int MAX_WORD_SIZE = 32; //rozsadnie

void checkForWord(wchar_t* word, struct dictionary* dict, bool ifDbg, int w, int z)
//zakladamy ze na wejsciu jest slowo w sensie samych liter z jezyka pl
{
	//printf("word do checkowania: >%ls<\n", word);
	if(word[0] != '\0')
	{
		if(dictionary_find(dict, word))
		{
			printf("%ls", word);
		}
		else
		{
			printf("#%ls", word);
			if(ifDbg)
			{
				struct word_list list;
				word_list_init(&list);
				dictionary_hints(dict, word, &list);
				fwprintf(stderr, L"%i, %i %ls:", w, z - wcslen(word) + 1,
					word);

				const wchar_t * const *a = word_list_get(&list);
				for (size_t i = 0; i < word_list_size(&list); ++i)
                    fwprintf(stderr, L" %ls", a[i]);
                
                fwprintf(stderr, L"\n");
				word_list_done(&list);
			}
		}
	}
}

void parseWord(struct dictionary* dict, wchar_t* line, bool ifDbg, int lineNr)
{
	wchar_t* word = (wchar_t*)malloc(256 * sizeof(wchar_t)); //rozsadny rozmiar

	printf("\nlinia: %ls\n", line);
	int i = 0;
	int j = 0;
	while(line[i] != '\0')
	{	
		if(iswalpha(line[i]))
		{
			word[j] = line[i];
			j++;
		}	
		else
		{
			word[j] = '\0';
			wchar_t* onlyWord = (wchar_t*)malloc((j + 2) * sizeof(wchar_t));
			wcscpy(onlyWord, word);
			checkForWord(word, dict, ifDbg, lineNr, i);
			free(word);
			word = (wchar_t*)malloc(256 * sizeof(wchar_t));
			while(!iswalpha(line[i]) && line[i] != '\0')
			{
				printf("%lc", line[i]);
				i++;
			}
			if (line[i] != '\0')
				i--;
			j = 0;
		}

		i++;	
	}
	//word[i] = '\0';
	//checkForWord(word, dict, ifDbg, lineNr, i);
	free(word);
}





int main(int argc, char const *argv[])
{
	setlocale(LC_ALL, "pl_PL.UTF-8");

	bool ddebug = false;
	bool parseInput = true;
	FILE* dictLocation = NULL;
	switch (argc)
	{
		case 2:
			dictLocation = fopen(argv[1], "r");
			if (dictLocation == NULL)
			{
				printf("ERR: nie udalo sie otworzyc slownika\n");
				parseInput = false;
			}
			break;
		case 3:
			if(strcmp("-v", argv[1]) == 0)
				ddebug = true;
			else
			{
				printf("ERR: zla opcja %s\n", argv[1]);
				parseInput = false;
			}
			dictLocation = fopen(argv[2], "r");
			if (dictLocation == NULL)
			{
				printf("ERR: nie udalo sie otworzyc slownika\n");
				parseInput = false;
			}
			break;

		default:
			printf("ERR: zla liczba argumentow\n");
			break;
	}

	if (parseInput)
	{
		struct word_list list;
		word_list_init(&list);
		struct dictionary* dict = dictionary_load(dictLocation);

		wchar_t line[256];
		int lineNr = 0;
		while(fgetws(line, sizeof(line), stdin))
		{
			lineNr++;
			parseWord(dict, line, ddebug, lineNr);
		}
		dictionary_done(dict);
		word_list_done(&list);

	}

	return 0;
}