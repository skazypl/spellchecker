/** @defgroup dict-check Moduł dict-check
	Klient do sprawdzania tekstu z wejścia na występowanie poszczególnych
	słów w słowniku.
	Korzysta z biblioteki @ref dictionary.
  */
/** @file
    Główny plik modułu dict-editor
    @ingroup dict-check
	@author Jarosław Socha <js347267@students.mimuw.edu.pl>

  */

#include <stdio.h>
#include <stdlib.h>
#include <dictionary.h>
#include <string.h>
#include <locale.h>
#include <wctype.h>
#include <errno.h>

const int MAX_LINE_SIZE = 1024; ///< Rozsądnie maksymalna zakładana długość słowa.

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
	//printf(">%ls<\n", word);
	if(wcslen(word) > 0)//if(word[0] != '\0')
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
				dictionary_hints(dict, word, &list);
				fwprintf(stderr, L"%i,%i %ls: ", w, z - wcslen(word) + 1,
					word);

				const wchar_t * const *a = word_list_get(&list);
				for (size_t i = 0; i < word_list_size(&list); ++i)
				{
                    fwprintf(stderr, L"%ls", a[i]);
                    if((i != word_list_size(&list)) &&
                    	(word_list_size(&list) != 1))
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
	wchar_t* word = (wchar_t*)malloc(MAX_LINE_SIZE * sizeof(wchar_t));
	//rozsadny rozmiar
	//todo - const!

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
			i++;
		}	
		else
		{
			word[j] = '\0';
			wchar_t* onlyWord = (wchar_t*)malloc((j + 1) * sizeof(wchar_t));
			/// ^ moze j+1? todo wazne!
			// for (int k = 0; word[k] >= 0; k++)
			// {
			// 	onlyWord[k] = word[k];
			// 	if(word[k] == 0)
			// 		break;
			// }

			wcscpy(onlyWord, word);
			checkForWord(onlyWord, dict, ifDbg, lineNr, i);
			free(onlyWord);

			while(!iswalpha(line[i]) && (line[i] != L'\0'))
			{
				printf("%lc", line[i]);
				i++;
			}
			j = 0;
		}
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
			parseInput = false;
			break;
	}

	if (parseInput)
	{
		//struct word_list list;
		struct dictionary* dict = dictionary_load(dictLocation);
		if(dict == NULL)
		{
			printf("Blad w otwieraniu slownika!\n");
		}
		else
		{
			wchar_t* line = malloc(sizeof(wchar_t) * MAX_LINE_SIZE);
			for (int i = 0; i < MAX_LINE_SIZE; ++i)
					line[i] = L'\0'; //zeruj inaczej. po co w ogole ten malloc?

			int lineNr = 0;
			while(fgetws(line, MAX_LINE_SIZE, stdin))
			{
				lineNr++;
				int length = 0;
				int j = 0;
				while(line[j] != L'\0')
				{
					length++;
					j++;
				}

				wchar_t* onlyLine = malloc((length + 2) * sizeof(wchar_t));
				for (int i = 0; i < length; ++i)
					onlyLine[i] = line[i];

				onlyLine[length] = L'\0';

				parseWord(dict, onlyLine, ddebug, lineNr);

				for (int i = 0; i < MAX_LINE_SIZE; ++i)
					line[i] = L'\0';

				free(onlyLine);

			}
			dictionary_done(dict);
			free(line);
		}
		fclose(dictLocation);
	}

	return 0;
}