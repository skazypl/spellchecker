#include <stdio.h>
#include <stdlib.h>
#include <dictionary.h>
#include <string.h>
#include <locale.h>
#include <wctype.h>
#include <errno.h>

const int MAX_WORD_SIZE = 64; ///< Rozsądnie maksymalna zakładana długość słowa.

/**
	Sprawdza czy w argumentowanym słowniku jest zadane słowo w określonym 
	wierszu	i pozycji.
	@param[in] word Szukane słowo.
	@param[in] dict Rozważany słownik.
	@param[in] ifDbg Opcja zależna od -v w wywołaniu programu.
	@param[in] w Numer wiersza słowa.
	@param[in] z Numer znaku w wierszu, od którego zaczyna się słowo.
	*/

void checkForWord(wchar_t* word, struct dictionary* dict, bool ifDbg, int w, 
	int z)
//zakladamy ze na wejsciu jest slowo w sensie samych liter z jezyka pl
{
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
				fwprintf(stderr, L"%i,%i %ls: ", w, z - wcslen(word) + 1,
					word);

				const wchar_t * const *a = word_list_get(&list);
				for (size_t i = 0; i < word_list_size(&list); ++i)
				{
                    fwprintf(stderr, L"%ls", a[i]);
                    if((i != word_list_size(&list)) && (word_list_size(&list) != 1))
                    	fwprintf(stderr, L" ");
                }
                
                fwprintf(stderr, L"\n");
				word_list_done(&list);
			}
		}
	}
}

/**
	Parsuje kolejne słowo z linii, tzn spójny ciąg znaków z danego alfabetu, 
	i wywołuje na nim funkcję checkForWord.
	@param[in] dict Słownik.
	@param[in] line Linia.
	@param[in] ifDbg Opcja zależna od -v w wywołaniu programu.
	@param[in] lineNr Numer parsowanej linii.
*/

void parseWord(struct dictionary* dict, wchar_t* line, bool ifDbg, int lineNr)
{
	wchar_t* word = (wchar_t*)malloc(256 * sizeof(wchar_t)); //rozsadny rozmiar

	int i = 0;
	int j = 0;
	if(wcslen(line) == 0)
		return;

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
			checkForWord(onlyWord, dict, ifDbg, lineNr, i);
			free(onlyWord);

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
	free(word);
}

/**
	Funkcja main.
	Główna funkcja programu.
*/

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
		for (int i = 0; i < 256; ++i)
				line[i] = '\0';

		int lineNr = 0;
		while(fgetws(line, sizeof(line), stdin))
		{
			lineNr++;
			int length = 0;
			int i = -1;
			while(line[i++] != '\0')
				length++;
			i--;

			wchar_t onlyLine[length + 1];
			for (int i = 0; i < length; ++i)
				onlyLine[i] = line[i];
			onlyLine[length] = '\0';

			parseWord(dict, onlyLine, ddebug, lineNr);
			for (int i = 0; i < 256; ++i)
				line[i] = '\0';

		}
		dictionary_done(dict);
		word_list_done(&list);
		fclose(dictLocation);
	}

	return 0;
}