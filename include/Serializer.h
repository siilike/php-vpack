#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include "php_vpack.h"

extern zend_class_entry* vpack_serializer_ce;

ZEND_BEGIN_ARG_INFO_EX(vpack_serializer_serialize, 0, 0, 3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_serializer_deserialize, 0, 0, 2)
ZEND_END_ARG_INFO()

static zend_function_entry vpack_serializer_me[] =
{
	PHP_ABSTRACT_ME(Serializer, serialize, vpack_serializer_serialize)
	PHP_ABSTRACT_ME(Serializer, deserialize, vpack_serializer_deserialize)
	PHP_FE_END
};

void inline vpack_register_interface_serializer(INIT_FUNC_ARGS)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "VPack", "Serializer", vpack_serializer_me);
	vpack_serializer_ce = zend_register_internal_interface(&ce);
}

#endif
