#ifndef VPACK_EXCEPTIONS_H_
#define VPACK_EXCEPTIONS_H_

#include "php_vpack.h"

extern "C" {

#include <Zend/zend_exceptions.h>

}

static zend_class_entry* vpack_exception_ce;

static zend_function_entry vpack_exception_me[] =
{
	PHP_FE_END
};

void inline vpack_register_exception(INIT_FUNC_ARGS)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "VPack", "Exception", vpack_exception_me);
	vpack_exception_ce = zend_register_internal_class_ex(&ce, zend_exception_get_default());

	zend_class_implements(vpack_exception_ce TSRMLS_CC, 1, zend_ce_throwable);
}

#endif
