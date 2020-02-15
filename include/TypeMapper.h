#ifndef BUILDER_H_
#define BUILDER_H_

#include "php_vpack.h"

extern zend_class_entry* mapper_ce;

typedef struct php_vpack_mapper_t
{
	VPACK_ZEND_OBJECT_PRE
	zval tagToClassMap;
	zval classToTagMap;
	zval serializers;
	zval deserializers;
	zval builder;
	VPACK_ZEND_OBJECT_POST
} php_vpack_mapper_t;

#define Z_MAPPER_OBJ_P(zv) (php_mapper_fetch_object(Z_OBJ_P(zv)))
#define Z_OBJ_MAPPER(zo) (php_mapper_fetch_object(zo))

static inline php_vpack_mapper_t* php_mapper_fetch_object(zend_object* obj)
{
	return (php_vpack_mapper_t*) ((char*) obj - XtOffsetOf(php_vpack_mapper_t, std));
}

void mapper_init_ce(INIT_FUNC_ARGS);
void serialize(char* name, size_t nameLen, uint64_t tag, zval* zmapper, VpackBuilder *b, zval* input);

#endif
