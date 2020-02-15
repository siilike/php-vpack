#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include "php_vpack.h"

extern zend_class_entry* vpack_serializable_ce;

ZEND_BEGIN_ARG_INFO_EX(vpack_serializable_from, 0, 0, 2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_serializable_to, 0, 0, 2)
ZEND_END_ARG_INFO()

static zend_function_entry vpack_serializable_me[] =
{
	PHP_ABSTRACT_ME(Serializable, toVPack, vpack_serializable_to)
    ZEND_FENTRY(fromVPack, NULL, vpack_serializable_from, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT | ZEND_ACC_STATIC)
	PHP_FE_END
};

void inline vpack_register_interface_serializable(INIT_FUNC_ARGS)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "VPack", "Serializable", vpack_serializable_me);
	vpack_serializable_ce = zend_register_internal_interface(&ce);

	zend_class_implements(vpack_serializable_ce TSRMLS_CC, 1, vpack_tagged_ce);
}

#endif
