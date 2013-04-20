#include <typeparser.h>
#include <retval.h>
#include <string.h>
#include <log.h>
#include <vector.h>
#include <tree.h>
#include <scanner.h>
#include <stdio.h>
#include <strpool.h>
static GlimpseTypeAlias_t _glimpse_typeparser_alias_table[GLIMPSE_MAX_TYPE_ALIAS];
static int _glimpse_typeparser_alias_count = 0;
static inline GlimpseTypeDesc_t* _glimpse_typeparser_find_alias(const char* name)
{
	int i;
	for(i = 0; i < _glimpse_typeparser_alias_count; i ++)
		if(0 == strcmp(_glimpse_typeparser_alias_table[i].name, name))
			return _glimpse_typeparser_alias_table[i].type;
	return NULL;
}
int glimpse_typeparser_alias(GlimpseTypeDesc_t* desc, const char* name)
{
	GlimpseTypeHandler_t* handler = glimpse_typesystem_query(desc);
	if(NULL == handler) return EUNKNOWN;
	if(NULL != _glimpse_typeparser_find_alias(name))
	{
		GLIMPSE_LOG_ERROR("failed to alias name with type, because name %s confilicted", name);
		return EINVAILDARG;
	}
	_glimpse_typeparser_alias_table[_glimpse_typeparser_alias_count].type = handler->type;  /* all handler should be queried */
	_glimpse_typeparser_alias_table[_glimpse_typeparser_alias_count].name = name;
	return ESUCCESS;
}

static inline void _glimpse_typeparser_process_property(const char* name, const char* key, const char* value, GlimpseTypeDesc_t* desc)
{
	if(strcmp(name,"vector") == 0)
	{
		if(strcmp(name, "seperator") == 0)
			desc->param.vector.sep = value[0];
	}
	else if(strcmp(name,"log") == 0)
	{
		if(strcmp(name, "name") == 0)
			desc->param.sublog.tree = glimpse_scanner_find_tree(value);
	}
	else if(strcmp(name,"map") == 0)
	{
		//TODO
	}
	else
	{
		glimpse_typesystem_typedesc_set_property(desc, key, value);
	}

}
static const char* _glimpse_typeparser_parse_type_imp(const char* text, GlimpseTypeDesc_t** desc)
{
	int status = 0;
	int comment = 0;
	int builtin = GLIMPSE_TYPE_BUILTIN_NONE;
	char name[1024];
	char key[1024];
	char value[1024];
	char* base;
	size_t size;
	char* p, c, prev;
#define IN(c,l,r) ((l) <= (c) && (c) <= (r))
#define IN2(c,l1,r1,l2,r2) (IN(c,l1,r1) || IN(c,l2,r2))
#define IN3(c,l1,r1,l2,r2,l3,r3) (IN2(c,l1,r1,l2,r2) || IN(c,l3,r3))
#define OUTPUT(var) do{ p = var; size = sizeof(var); base=var; var[0] = 0; } while(0)
#define APPEND(fmt, arg...) p += snprintf(p, size - (p - base), fmt, ##arg)
#define IGNSPC if(c == ' ' || c == '\n' || c == '\t' || c =='\r') continue
	for(;text && (c = *text); text ++)
	{
		if(comment && c != '\n') continue;
		comment = 0;
		if(status != 7 && status != 9 && c == '#')
		{
			comment = 1;
			continue;
		}
		switch(status)
		{
			case 0:   /* expect type name */
				IGNSPC;
				if(IN3(c,'a','z','A','Z','_','_')) 
				{
					OUTPUT(name);
					status = 1; 
					APPEND("%c",c);
				}
				else status = -1;  /* unexpected typename */
				break;
			case 1:  /* reading a type name */
				if(IN3(c,'a','z','A','Z','0','9') || c == '_') APPEND("%c",c);
				else status = 2;
				break;
			case 2: /* except { */
				IGNSPC;
				if(c == '{') 
				{
					status = 3;
					if(strcmp(name,"vector") == 0) builtin = GLIMPSE_TYPE_BUILTIN_VECTOR;
					else if(strcmp(name,"sublog") == 0) builtin = GLIMPSE_TYPE_BUILTIN_SUBLOG;
					else if(strcmp(name,"map") == 0) builtin = GLIMPSE_TYPE_BUILTIN_MAP;
					else
					{
						(*desc) = glimpse_typesystem_typedesc_new(glimpse_typesystem_sizeof_typegroup_prop(name));
						(**desc).builtin_type = builtin;
						(**desc).param.normal.group = glimpse_strpool_new(name);
						break;
					}
					(*desc) = glimpse_typesystem_typedesc_new(0);
					(**desc).builtin_type = builtin;
				}
				else goto END;  /* if a typename not followed by { */
				break;
			case 3: /* type body or }, searching for the first char of key */
				IGNSPC;
				if(c == '}') goto END;
				if(IN2(c,'a','z','A','Z') || c == '_')
				{
					OUTPUT(key);
					APPEND("%c", c);
					status = 4;
				}
				else status = -2;  /* invaild key name */
				break;
			case 4: /* reading a key before the first char */
				if(IN3(c,'a','z','A','Z','0','9') || c == '_')
					APPEND("%c", c);
				else
					status = 5;
				break;
			case 5: /* got a key */
				IGNSPC;
				if(c == ':') status = 6;
				else status = -3; /* `:' excepted, but got .... */
				break;
			case 6: /* get ready to read value */
				IGNSPC;
				if(builtin == GLIMPSE_TYPE_BUILTIN_VECTOR && strcmp(name, "basetype") == 0) 
				{
					GlimpseTypeDesc_t* basetype = NULL;
					text = _glimpse_typeparser_parse_type_imp(text, &basetype);
					if(text) (**desc).param.vector.basetype = basetype;
					status = 3;
					break;
				}
				OUTPUT(value);
				if(c == ',') 
				{
					status = 3; /* empty value */
					_glimpse_typeparser_process_property(name, key, "", *desc);
				}
				else if(c == '}')
				{
					_glimpse_typeparser_process_property(name, key, "", *desc);
					goto END;
				}
				else if(c == '"')
					status = 7, prev = '"';
				else if(c == '\'')
					status = 7, prev = '\'';
				else if(IN3(c,'a','z','A','Z','0','9') || c == '.' || c == '+' || c == '-')
				{
					status = 8;
					APPEND("%c",c);
				}
				else status = -4; /* invaild value */
				break;
			case 7: /* reading inside a string */
				if(c == prev)
				{
					//got a string value
					_glimpse_typeparser_process_property(name, key, value, *desc);
					status = 3;
				}
				else if(c == '\\')
					status = 9; //escape 
				else APPEND("%c",c);
				break;
			case 8: /*reading normal value*/
				if(IN3(c,'a','z','A','Z','0','9') || c == '.' || c == '+' || c == '-')
					APPEND("%c",c);
				else if(c == ',')
				{
					_glimpse_typeparser_process_property(name, key, value, *desc);
					status = 3;
				}
				else if(c == '}')
				{
					_glimpse_typeparser_process_property(name, key, value, *desc);
					goto END;
				}
				else status = -4; /* reading normal value */
				break;
			case 9:
				switch(c)
				{
					case 'n': APPEND("\n"); break;
					case 't': APPEND("\t"); break;
					case 'r': APPEND("\r"); break;
					default:
							  APPEND("%c",c);
				}
				status = 7;
				break;
			default:
				goto ERR;

		}
	}
ERR:
		return NULL;
END:
	if(status == 1) /* aliased name */
	{
		(*desc) = _glimpse_typeparser_find_alias(name);

	}
	return text;
#undef IN
#undef IN2
#undef IN3
#undef OUTPUT
#undef APPEND
#undef IGNSPC
}
GlimpseTypeDesc_t* glimpse_typeparser_parse_type(const char* text)
{
}
