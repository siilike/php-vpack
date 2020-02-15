#ifndef VPACK_CLASSES_H
#define VPACK_CLASSES_H

extern "C" {

#include <php.h>

}

#define VPACK_ZEND_OBJECT_PRE
#define VPACK_ZEND_OBJECT_POST zend_object std;
#define VPACK_STRUCT_ZVAL zval

ZEND_BEGIN_ARG_INFO_EX(vpack_arginfo_none, 0, 0, 0)
ZEND_END_ARG_INFO()

#endif
