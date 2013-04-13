#include <TypeAPI.h>
#include <stdio.h>
#include <integer/integer.h>
#include <integer/fixlength.h>
void* alloc_int(void* data)
{
	GlimpseIntegerProperties_t* properties = (GlimpseIntegerProperties_t*)data;
	size_t size = 0;
	switch(properties->Size)
	{
		case GlimpseInteger8:
			size = 1;
			break;
		case GlimpseInteger16:
			size = 2;
			break;
		case GlimpseInteger32:
			size = 4;
			break;
		case GlimpseInteger64:
			size = 5;
			break;
		case GlimpseIntegerVariant:
			PLUGIN_LOG_ERROR("we can not support big integer yet");
			size = 0;
			break;
		default:
			PLUGIN_LOG_ERROR("unsupported integer type");
			size = 0;
	}
	if(0 == size) return NULL;
	return malloc(size);
}
void free_int(void* data, void* userdata)
{
	free(data);
}
int resolve_int(const GlimpseTypeDesc_t* type, GlimpseTypeHandler_t* handler)
{
	if(NULL == type || NULL == handler) return -1;
	GlimpseIntegerProperties_t* properties = (GlimpseIntegerProperties_t*)type->properties;
	handler->parse_data = properties;
	handler->alloc_data = properties;
	handler->free_data = properties;
	handler->alloc = alloc_int;
	handler->free = free_int; 
	switch(properties->Size)
	{
/* Local Macro, undefined after used */
#define _CASE_SIZE(len)\
		case GlimpseInteger##len:\
			switch(properties->Representation)\
			{\
				case GlimpseIntegerBin:\
					if(properties->Signed) handler->parse = glimpse_integer_fixlength_bin_i##len;\
					else handler->parse = glimpse_integer_fixlength_bin_u##len;\
					break;\
				case GlimpseIntegerOct:\
					if(properties->Signed) handler->parse = glimpse_integer_fixlength_oct_i##len;\
					else handler->parse = glimpse_integer_fixlength_oct_u##len;\
					break;\
				case GlimpseIntegerDec:\
					if(properties->Signed) handler->parse = glimpse_integer_fixlength_dec_i##len;\
					else handler->parse = glimpse_integer_fixlength_dec_u##len;\
					break;\
				case GlimpseIntegerHex:\
					if(properties->Signed) handler->parse = glimpse_integer_fixlength_hex_i##len;\
					else handler->parse = glimpse_integer_fixlength_hex_u##len;\
					break;\
				default:\
					return -1;\
			}\
			break
		_CASE_SIZE(8);
		_CASE_SIZE(16);
		_CASE_SIZE(32);
		_CASE_SIZE(64);
		case GlimpseIntegerVariant:
			//TODO
			PLUGIN_LOG_WARNING("Integer Plugin does not support big integer yet");
			return -1;
			break;
		default:
			return -1;
#undef _CASE_SIZE
	}
	PLUGIN_LOG_DEBUG("Integer, Size = %d, Signed = %d, Representation = %d Assigned to parser at <0x%x>", 
	                 properties->Size, 
					 properties->Signed, 
					 properties->Representation,
					 handler->parse);
	return 0;
}
GlimpseTypeGroup_t int_type_group = {
	.name = "integer",
	.resolve = resolve_int
};
int init()
{
	RegisterTypeGroup(&int_type_group);
	return 0;
}
GlimpsePluginMetaData(TypeAPI)
	GlimpsePluginName("integer");
	GlimpsePluginVersion(0,0,1);
	GlimpsePluginAPICallBack(TypeAPI, OnInitialized) = init; 
GlimpsePluginEndMetaData