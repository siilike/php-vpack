#ifndef SLICE_H_
#define SLICE_H_

#include "php_vpack.h"

extern zend_class_entry* slice_ce;

typedef struct php_vpack_slice_t
{
	VPACK_ZEND_OBJECT_PRE
	zval startZval;
	const uint8_t *start;
	long offset;
	VPACK_ZEND_OBJECT_POST
} php_vpack_slice_t;

#define Z_SLICE_OBJ_P(zv) (php_slice_fetch_object(Z_OBJ_P(zv)))
#define Z_OBJ_SLICE(zo) (php_slice_fetch_object(zo))

static inline php_vpack_slice_t* php_slice_fetch_object(zend_object* obj)
{
	return (php_vpack_slice_t*) ((char*) obj - XtOffsetOf(php_vpack_slice_t, std));
}

void slice_init_ce(INIT_FUNC_ARGS);
void slice_ctor(zval* slice, const uint8_t* start, zend_long offset);
void new_slice_ref(zval *v, zval *r, long offset);
void new_slice_ref_slice(zval *v, php_vpack_slice_t *d, VpackSlice &s);
void new_slice_ref_zval(zval *v, zval *d, VpackSlice &s);

#endif
