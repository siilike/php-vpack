#ifndef TAGGED_H_
#define TAGGED_H_

#include "php_vpack.h"

extern zend_class_entry* vpack_tagged_ce;

static zend_function_entry vpack_tagged_me[] =
{
	PHP_ABSTRACT_ME(Tagged, getTagId, vpack_arginfo_none)
	PHP_FE_END
};

void inline vpack_register_interface_tagged(INIT_FUNC_ARGS)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "VPack", "Tagged", vpack_tagged_me);
	vpack_tagged_ce = zend_register_internal_interface(&ce);
}

#endif
