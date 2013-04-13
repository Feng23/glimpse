#ifndef __TREE_H__
#define __TREE_H__
#include <log.h>
#include <chartable.h>
#include <typesystem.h>
#include <stdint.h>
typedef struct _glimpse_trie_node{
	uint8_t term;
	union{
		GlimpseCharTable_t* child;
		struct{
			GlimpseTypeHandler_t* handler;
			void* data;
		} terminus;
	} s;
} GlimpseTrieNode_t;
/*Trie Node manipulation*/
GlimpseTrieNode_t* glimpse_tree_trienode_new(int terminus);   /* terminus indicates if the node a terminal node */
void glimpse_tree_trienode_free(GlimpseTrieNode_t* node);

typedef struct _glimpse_parse_tree{
	const char* sep_f;  /* seperator between feilds */
	const char* sep_kv; /* seperator between key and value */
	GlimpseTrieNode_t* root;
} GlimpseParseTree_t;
/* Tree manipulation */
GlimpseParseTree_t* glimpse_tree_new(const char* sep_f, const char* sep_kv);
void glimpse_tree_free(GlimpseParseTree_t* tree);
int glimpse_tree_set_sperator(GlimpseParseTree_t* tree, const char* f, const char* kv);
int glimpse_tree_insert(GlimpseParseTree_t* tree, const char* field, GlimpseTypeDesc_t* type);
/* Operation */
typedef GlimpseTrieNode_t* GlimpseParserStatus_t;
static inline GlimpseParserStatus_t glimpse_tree_scan_start(GlimpseParseTree_t* tree)
{
	if(NULL == tree) return NULL;
	return tree->root;
}
static inline GlimpseParserStatus_t glimpse_tree_scan(GlimpseParserStatus_t status, char ch)
{
	if(1 == status->term) return NULL; /* terminus, can not walk down */
	return (GlimpseParserStatus_t)glimpse_chartable_find(status->s.child, (uint8_t)ch);
}
#endif