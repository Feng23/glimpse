#ifndef __TYPEFLAG_H__
#define __TYPEFLAG_H__
#include <future.h>
#include <typesystem.h>
#include <def.h>
/* interfaces for process log,vector,map as normal type */

/* vector interface */
void* glimpse_typeflag_vector_alloc(void* userdata);
int glimpse_typeflag_vector_init(void* data, void* userdata);
int glimpse_typeflag_vector_finalize(void* data, void* userdata);
int glimpse_typeflag_vector_free(void* data, void* userdata);
const char* glimpse_typeflag_vector_parse(const char* text, void* result, void* user_data, void* thread_data);
char* glimpse_typeflag_vector_tostring(GlimpseTypeHandler_t* type, char* buffer, size_t size);

void* glimpse_typeflag_sublog_alloc(void* userdata);
int glimpse_typeflag_sublog_init(void* data, void* userdata);
int glimpse_typeflag_sublog_finalize(void* data, void* userdata);
int glimpse_typeflag_sublog_free(void* data, void* userdata);
const char* glimpse_typeflag_sublog_parse(const char* text, void* result, void* user_data, void* thread_data);
char* glimpse_typeflag_sublog_tostring(GlimpseTypeHandler_t* type, char* buffer, size_t size);
#endif
