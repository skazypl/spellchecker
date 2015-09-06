/**
	@file
	Atrapy funkcji I/O i alokujących pamięć.
	
	@ingroup dictionary
	@author Jarosław Socha <js347267@students.mimuw.edu.pl>	
*/

#ifndef IO_MOCK_H
#define IO_MOCK_H

#ifdef UNIT_TESTING

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#ifdef fprintf
#undef fprintf
#endif /*fprintf*/

extern int new_fprintf(FILE* stream, const char *format, int i, wchar_t wc);
#define fprintf new_fprintf

#ifdef fscanf
#undef fscanf
#endif /*fscanf*/

extern int new_fscanf(FILE* stream, const char *format, int* i, wchar_t* wc);
#define fscanf new_fscanf


#ifdef malloc
#undef malloc
#endif /* malloc */
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#ifdef free
#undef free
#endif /* free */
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)
void* _test_malloc(const size_t size,
                   const char* file, const int line);
void _test_free(void* const ptr, const char* file, const int line);

#endif /*UNIT_TESTING*/
#endif /*IO_MOCK*/
