/** @file
  Implementacja listy słów.

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-05-10
 */

#include "word_list.h"

/** @name Elementy interfejsu 
   @{
 */

void word_list_init(struct word_list *list)
{
    list->size = 0;
    list->buffer_size = 1;
    list->array = malloc(sizeof(wchar_t*));
}

void word_list_done(struct word_list *list)
{
	for (int i = 0; i < list->size; ++i)
		free(list->array[i]);
	free(list->array);
	list->array = NULL;
	//free(list);
}

int word_list_add(struct word_list *list, const wchar_t *word)
{
	if(word == NULL)
		return 0;

	++(list->size);
	if(list->size > list->buffer_size)
	{	
		wchar_t **newArray = 
			malloc(sizeof(wchar_t*) * list->buffer_size * 2);
		//memcpy(newArray, list->array, sizeof(wchar_t) * list->buffer_size);
		for (int i = 0; i < list->buffer_size; ++i)
			newArray[i] = list->array[i];
		for (int i = list->buffer_size; i < 2 * list->buffer_size; ++i)
			newArray[i] = NULL;
		
		list->buffer_size *= 2;
		free(list->array);
		list->array = newArray;
	}
	wchar_t* wordCopy = malloc((wcslen(word) + 1) * sizeof(wchar_t));
	wcscpy(wordCopy, word);
	list->array[list->size - 1] = wordCopy;
	//list->bufferSize += (wcslen(word) + 1);

    
    return 1;
}

/**@}*/
