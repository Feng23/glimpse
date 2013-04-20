#include <strpool.h>
#include <vector.h>
#include <malloc.h>
#include <string.h>
static GlimpseVector_t* _glimpse_strpool_table;
int glimpse_strpool_init()
{
	_glimpse_strpool_table = (GlimpseVector_t*)glimpse_vector_new(sizeof(char*));
	if(NULL == _glimpse_strpool_table) return EUNKNOWN;
	return ESUCCESS;
}
int glimpse_strpool_cleanup()
{
	int i = 0;
	for(i = 0; i < _glimpse_strpool_table->size; i ++)
	{
		void* str = *(void**)glimpse_vector_get(_glimpse_strpool_table, i);
		free(str);
	}
	glimpse_vector_free(_glimpse_strpool_table);
	return ESUCCESS;
}
char* glimpse_strpool_new(const char* str)
{
	if(NULL == str) return NULL;
	size_t len = strlen(str);
	void* mem = malloc(len + 1);
	memcpy(mem, str, len + 1);
	if(glimpse_vector_push(_glimpse_strpool_table, &mem) != ESUCCESS)
	{
		free(mem);
		return NULL;
	}
	return (char*)mem;
}