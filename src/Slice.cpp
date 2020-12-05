
extern "C" {

#include <php.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_exceptions.h>
#include <ext/spl/spl_iterators.h>

}

#include "php_vpack.h"
#include "Slice.h"
#include "vpack_classes.h"


static zend_object_handlers slice_handler;

typedef struct slice_iterator
{
	zend_object_iterator iter;
	php_vpack_slice_t *slice;
	VpackArrayIterator *arrayIterator;
	VpackObjectIterator *objectIterator;
	bool failed;
	zval current;
} slice_iterator;


void slice_ctor(zval* slice, const uint8_t* start, zend_long offset)
{
	php_vpack_slice_t* intern = Z_SLICE_OBJ_P(slice);

	intern->start = start + offset;
	intern->offset = offset;
}

void new_slice_ref(zval *v, zval *r, long offset)
{
	if(object_init_ex(v, slice_ce) == FAILURE)
	{
		zend_error(E_ERROR, "Failed to create Slice object");
	}

	zval ret;
	zval fname;
	zval params[2];
	zval zoffset;
	int result;

	ZVAL_STRING(&fname, "__construct");
	ZVAL_LONG(&zoffset, offset);

	ZVAL_COPY(&params[0], r);
	ZVAL_LONG(&params[1], offset);

	zend_fcall_info fci;

	fci.size = sizeof(fci);
	fci.object = Z_OBJ_P(v);
	ZVAL_COPY_VALUE(&fci.function_name, &fname);
	fci.retval = &ret;
	fci.param_count = 2;
	fci.params = &params[0];

#if PHP_VERSION_ID >= 80000
	fci.named_params = 0;
#endif

	result = zend_call_function(&fci, NULL);

	zval_ptr_dtor(&ret);
	zval_ptr_dtor(&fname);
	zval_ptr_dtor(&params[0]);
	zval_ptr_dtor(&params[1]);

	if(result != SUCCESS)
	{
		zend_error(E_ERROR, "Failed to initialize Slice object");
	}
}

void new_slice_ref_slice(zval *v, php_vpack_slice_t *d, VpackSlice &s)
{
	long offset = abs(d->start - s.start()) + d->offset;

	new_slice_ref(v, &d->startZval, offset);
}

void new_slice_ref_zval(zval *v, zval *d, VpackSlice &s)
{
	const uint8_t* start = (const uint8_t*) ZSTR_VAL(Z_STR_P(d));
	long offset = abs(start - s.start());

	new_slice_ref(v, d, offset);
}


PHP_METHOD(Slice, __construct)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	zval*              input;
	zend_long          offset = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &input, &offset) == FAILURE)
	{
		zend_throw_exception_ex(vpack_exception_ce, 0, "No input supplied for Slice");
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	int type = Z_TYPE_P(input);

	if(type == IS_STRING)
	{
		ZVAL_COPY(&intern->startZval, input);

		slice_ctor(getThis(), (const uint8_t*) ZSTR_VAL(Z_STR(intern->startZval)), offset);
		return;
	}

	zend_throw_exception_ex(vpack_exception_ce, 0, "Slice input has to be a string");

	VPACK_CATCH
}

PHP_METHOD(Slice, value)
{
	VPACK_TRY

	php_vpack_slice_t *intern;
	zval               ret;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	if(s.isTagged())
	{
		new_slice_ref_slice(&ret, intern, s);

		RETURN_ZVAL(&ret, 0, 1);
	}
	else
	{
		Z_ADDREF_P(getThis());

		RETURN_ZVAL(getThis(), 0, 0);
	}

	VPACK_CATCH
}

PHP_METHOD(Slice, getFirstTag)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.getFirstTag());

	VPACK_CATCH
}

PHP_METHOD(Slice, getTags)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	std::vector<uint64_t> tags = s.getTags();

    array_init_size(return_value, tags.size());

    for(uint64_t &tag : tags)
    {
		add_next_index_long(return_value, tag);
	}

	VPACK_CATCH
}

PHP_METHOD(Slice, hasTag)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	long               id;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.hasTag(id));

	VPACK_CATCH
}

PHP_METHOD(Slice, type)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG((uint8_t) s.type());

	VPACK_CATCH
}

PHP_METHOD(Slice, typeName)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_STRING(s.typeName());

	VPACK_CATCH
}

PHP_METHOD(Slice, hash)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.hash());

	VPACK_CATCH
}

PHP_METHOD(Slice, hash32)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.hash32());

	VPACK_CATCH
}

PHP_METHOD(Slice, hashSlow)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.hashSlow());

	VPACK_CATCH
}

PHP_METHOD(Slice, normalizedHash)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.normalizedHash());

	VPACK_CATCH
}

PHP_METHOD(Slice, normalizedHash32)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.normalizedHash32());

	VPACK_CATCH
}

PHP_METHOD(Slice, hashString)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.hashString());

	VPACK_CATCH
}

PHP_METHOD(Slice, hashString32)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.hashString32());

	VPACK_CATCH
}

PHP_METHOD(Slice, isType)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	long               id;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isType((VpackValueType) id));

	VPACK_CATCH
}


PHP_METHOD(Slice, isNone)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isNone());

	VPACK_CATCH
}

PHP_METHOD(Slice, isIllegal)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isIllegal());

	VPACK_CATCH
}

PHP_METHOD(Slice, isNull)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isNull());

	VPACK_CATCH
}

PHP_METHOD(Slice, isBoolean)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isBoolean());

	VPACK_CATCH
}

PHP_METHOD(Slice, isTrue)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isTrue());

	VPACK_CATCH
}

PHP_METHOD(Slice, isFalse)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isFalse());

	VPACK_CATCH
}

PHP_METHOD(Slice, isArray)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isArray());

	VPACK_CATCH
}

PHP_METHOD(Slice, isObject)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isObject());

	VPACK_CATCH
}

PHP_METHOD(Slice, isDouble)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isDouble());

	VPACK_CATCH
}

PHP_METHOD(Slice, isUTCDate)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isUTCDate());

	VPACK_CATCH
}

PHP_METHOD(Slice, isExternal)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isExternal());

	VPACK_CATCH
}

PHP_METHOD(Slice, isMinKey)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isMinKey());

	VPACK_CATCH
}

PHP_METHOD(Slice, isMaxKey)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isMaxKey());

	VPACK_CATCH
}

PHP_METHOD(Slice, isInt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isInt());

	VPACK_CATCH
}

PHP_METHOD(Slice, isUInt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isUInt());

	VPACK_CATCH
}

PHP_METHOD(Slice, isSmallInt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isSmallInt());

	VPACK_CATCH
}

PHP_METHOD(Slice, isBinary)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isBinary());

	VPACK_CATCH
}

PHP_METHOD(Slice, isBCD)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isBCD());

	VPACK_CATCH
}

PHP_METHOD(Slice, isCustom)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isCustom());

	VPACK_CATCH
}

PHP_METHOD(Slice, isTagged)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isTagged());

	VPACK_CATCH
}

PHP_METHOD(Slice, isInteger)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isInteger());

	VPACK_CATCH
}

PHP_METHOD(Slice, isNumber)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isNumber());

	VPACK_CATCH
}

PHP_METHOD(Slice, isSorted)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isSorted());

	VPACK_CATCH
}

PHP_METHOD(Slice, getBoolean)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.getBoolean());

	VPACK_CATCH
}

PHP_METHOD(Slice, getDouble)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_DOUBLE(s.getDouble());

	VPACK_CATCH
}

PHP_METHOD(Slice, at)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	zend_long          idx = 0;
	zval               ret;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &idx) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start).at(idx);

	new_slice_ref_slice(&ret, intern, s);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

PHP_METHOD(Slice, length)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.length());

	VPACK_CATCH
}

PHP_METHOD(Slice, keyAt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	zend_long          idx = 0;
	bool               translate = true;
	zval               ret;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l|b", &idx, &translate) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start).keyAt(idx, translate);

	new_slice_ref_slice(&ret, intern, s);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

PHP_METHOD(Slice, valueAt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	zend_long          idx = 0;
	zval               ret;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &idx) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start).valueAt(idx);

	new_slice_ref_slice(&ret, intern, s);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

PHP_METHOD(Slice, getNthValue)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	zend_long          idx = 0;
	zval               ret;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &idx) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start).getNthValue(idx);

	new_slice_ref_slice(&ret, intern, s);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

PHP_METHOD(Slice, get)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	char*              key;
	size_t             keyLen = 0;
	zval               ret;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keyLen) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start).get(key, keyLen);

	new_slice_ref_slice(&ret, intern, s);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

PHP_METHOD(Slice, hasKey)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	char*              key;
	size_t             keyLen;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keyLen) == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.hasKey(key, keyLen));

	VPACK_CATCH
}

// externals

PHP_METHOD(Slice, isEmptyArray)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isEmptyArray());

	VPACK_CATCH
}

PHP_METHOD(Slice, isEmptyObject)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_BOOL(s.isEmptyObject());

	VPACK_CATCH
}

PHP_METHOD(Slice, translate)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	zval               ret;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start).translate();

	new_slice_ref_slice(&ret, intern, s);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

PHP_METHOD(Slice, getInt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.getInt());

	VPACK_CATCH
}

PHP_METHOD(Slice, getUInt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.getUInt());

	VPACK_CATCH
}

PHP_METHOD(Slice, getSmallInt)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.getSmallInt());

	VPACK_CATCH
}

PHP_METHOD(Slice, getNumber)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	if(s.isDouble())
	{
		RETURN_DOUBLE(s.getDouble());
	}
	else
	{
		RETURN_LONG(s.getInt());
	}

	VPACK_CATCH
}

PHP_METHOD(Slice, getUTCDate)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	int64_t v = s.getUTCDate();

	RETURN_NULL(); // TODO

	VPACK_CATCH
}

PHP_METHOD(Slice, getString)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	VpackValueLength len;
	const char* str = s.getString(len);

	RETURN_NEW_STR(zend_string_init(str, len, 0));

	VPACK_CATCH
}

PHP_METHOD(Slice, getStringLength)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.getStringLength());

	VPACK_CATCH
}

PHP_METHOD(Slice, getBinary)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	VpackValueLength len;
	const char* val = (const char *) s.getBinary(len);

	RETURN_NEW_STR(zend_string_init(val, len, 0));

	VPACK_CATCH
}

PHP_METHOD(Slice, getBinaryLength)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.getBinaryLength());

	VPACK_CATCH
}

PHP_METHOD(Slice, byteSize)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.byteSize());

	VPACK_CATCH
}

PHP_METHOD(Slice, valueByteSize)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	RETURN_LONG(s.valueByteSize());

	VPACK_CATCH
}

// compareString

// isEqualString

PHP_METHOD(Slice, binaryEquals)
{
	VPACK_TRY

	php_vpack_slice_t *intern;
	php_vpack_slice_t *other;
	zval *input;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "O", &input, slice_ce) == FAILURE)
	{
		return;
	}

	if(!instanceof_function(Z_OBJCE_P(input), slice_ce))
	{
		RETURN_FALSE;
	}

	intern = Z_SLICE_OBJ_P(getThis());
	other = Z_SLICE_OBJ_P(input);

	RETURN_BOOL(VpackSlice(intern->start).binaryEquals(VpackSlice(other->start)));

	VPACK_CATCH
}

PHP_METHOD(Slice, toHex)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	std::string v = s.toHex();

	RETURN_NEW_STR(zend_string_init(v.c_str(), v.length(), 0));

	VPACK_CATCH
}

PHP_METHOD(Slice, toJson)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	intern = Z_SLICE_OBJ_P(getThis());

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	VpackSlice s = VpackSlice(intern->start);

	std::string v = s.toJson();

	RETURN_NEW_STR(zend_string_init(v.c_str(), v.length(), 0));

	VPACK_CATCH
}

PHP_METHOD(Slice, toString)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	std::string v = s.toString();

	RETURN_NEW_STR(zend_string_init(v.c_str(), v.length(), 0));

	VPACK_CATCH
}

PHP_METHOD(Slice, __toString)
{
	VPACK_TRY

	php_vpack_slice_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	std::string v = s.toString();

	RETURN_NEW_STR(zend_string_init(v.c_str(), v.length(), 0));

	VPACK_CATCH
}

PHP_METHOD(Slice, getBCD)
{
	VPACK_TRY

	php_vpack_slice_t* intern;
	int8_t sign;
	int32_t exponent;
	VpackValueLength mantissaLength;
	uint8_t const* value;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_SLICE_OBJ_P(getThis());

	VpackSlice s = VpackSlice(intern->start);

	value = s.getBCD(sign, exponent, mantissaLength);

	RETURN_NULL(); // TODO

	VPACK_CATCH
}

#if PHP_VERSION_ID >= 80000
	ZEND_METHOD(Slice, getIterator)
	{
		ZEND_PARSE_PARAMETERS_NONE();
		zend_create_internal_iterator_zval(return_value, ZEND_THIS);
	}
#endif

static zend_object* slice_create_object(zend_class_entry* class_type)
{
	php_vpack_slice_t* intern = NULL;

	intern = (php_vpack_slice_t*) zend_object_alloc(sizeof(php_vpack_slice_t), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->std.handlers = &slice_handler;

	return &intern->std;
}

static void slice_free_object(zend_object* object)
{
	php_vpack_slice_t* intern = Z_OBJ_SLICE(object);

	zend_object_std_dtor(&intern->std);

	if(!Z_ISUNDEF(intern->startZval))
	{
		zval_ptr_dtor(&intern->startZval);
	}

	efree(intern);
}

static void slice_iterator_invalidate(zend_object_iterator *iter)
{
	slice_iterator *d = (slice_iterator*) iter;

	if(!Z_ISUNDEF(d->current))
	{
		zval_ptr_dtor(&d->current);
		ZVAL_UNDEF(&d->current);
	}
}

static void slice_iterator_dtor(zend_object_iterator *iter)
{
	slice_iterator *d = (slice_iterator*) iter;

	slice_iterator_invalidate(iter);

	zval_ptr_dtor(&d->iter.data);
}

static int slice_iterator_valid(zend_object_iterator *iter)
{
	slice_iterator *d = (slice_iterator*) iter;

	if(d->arrayIterator)
	{
		return d->arrayIterator->index() < d->arrayIterator->size() ? SUCCESS : FAILURE;
	}

	return d->objectIterator->index() < d->objectIterator->size() ? SUCCESS : FAILURE;
}

static zval* slice_iterator_get_data(zend_object_iterator *iter)
{
	slice_iterator *d = (slice_iterator*) iter;

	VpackSlice s = d->arrayIterator ? d->arrayIterator->value() : d->objectIterator->value();

	switch(s.type())
	{
		case VpackValueType::Int:
		case VpackValueType::UInt:
		case VpackValueType::SmallInt:
			ZVAL_LONG(&d->current, s.getInt());
			break;
		case VpackValueType::String:
		{
			VpackValueLength length;
			char *str = (char*) s.getString(length);
			ZVAL_STRINGL(&d->current, str, length);
			break;
		}
		case VpackValueType::Bool:
			ZVAL_BOOL(&d->current, s.getBoolean());
			break;
		case VpackValueType::Object:
		case VpackValueType::Array:
		case VpackValueType::Tagged:
		{
			new_slice_ref_slice(&d->current, d->slice, s);
			break;
		}
		case VpackValueType::Null:
		case VpackValueType::None:
			ZVAL_NULL(&d->current);
			break;
		case VpackValueType::BCD: // TODO
		{
			ZVAL_NULL(&d->current);
			break;
		}
		default:
			php_error_docref(NULL, E_WARNING, "Unsupported VPack type '%s'", s.typeName());
			ZVAL_NULL(&d->current);
			break;
	}

	return &d->current;
}

static void slice_iterator_get_key(zend_object_iterator *iter, zval *key)
{
	slice_iterator *d = (slice_iterator*) iter;

	if(d->failed)
	{
		ZVAL_NULL(key);
	}
	else if(d->arrayIterator)
	{
		ZVAL_LONG(key, d->arrayIterator->index());
	}
	else
	{
		VpackSlice k = d->objectIterator->key();
		VpackValueLength kLen;
		char* str = (char*) k.getString(kLen);

		ZVAL_STRINGL(key, str, kLen);
	}
}

static void slice_iterator_move_forwards(zend_object_iterator *iter)
{
	slice_iterator *d = (slice_iterator*) iter;

	if(d->arrayIterator)
	{
		if(d->arrayIterator->index() < d->arrayIterator->size())
		{
			(*d->arrayIterator)++;
		}
		else
		{
			d->failed = true;
		}
	}
	else
	{
		if(d->objectIterator->index() < d->objectIterator->size())
		{
			(*d->objectIterator)++;
		}
		else
		{
			d->failed = true;
		}
	}

	slice_iterator_invalidate(iter);
}

static void slice_iterator_rewind(zend_object_iterator *iter)
{
	slice_iterator *d = (slice_iterator*) iter;

	if(d->arrayIterator)
	{
		d->arrayIterator->reset();
	}
	else
	{
		d->objectIterator->reset();
	}

	d->failed = false;

	slice_iterator_invalidate(iter);
}

static zend_object_iterator_funcs slice_iterator_funcs =
{
	slice_iterator_dtor,
	slice_iterator_valid,
	slice_iterator_get_data,
	slice_iterator_get_key,
	slice_iterator_move_forwards,
	slice_iterator_rewind,
	slice_iterator_invalidate
};

static zend_object_iterator* slice_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	php_vpack_slice_t *v;
	slice_iterator *iter;

	if(by_ref)
	{
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	v = (php_vpack_slice_t*) Z_SLICE_OBJ_P(object);
	iter = (slice_iterator*) emalloc(sizeof(slice_iterator));

	zend_iterator_init(&iter->iter);
	Z_ADDREF_P(object);

	ZVAL_OBJ(&iter->iter.data, Z_OBJ_P(object));
	iter->iter.funcs = &slice_iterator_funcs;

	iter->slice = v;
	ZVAL_UNDEF(&iter->current);

	VpackSlice *s = new VpackSlice(iter->slice->start);

	if(s->isArray())
	{
		iter->arrayIterator = new VpackArrayIterator(*s);
		iter->objectIterator = NULL;
	}
	else if(s->isObject())
	{
		iter->objectIterator = new VpackObjectIterator(*s);
		iter->arrayIterator = NULL;
	}
	else
	{
		zend_error(E_ERROR, "Slice is not an array nor an object");
	}

	return (zend_object_iterator *) iter;
}


ZEND_BEGIN_ARG_INFO_EX(vpack_slice_const, 0, 0, 1)
ZEND_ARG_INFO(0, input)
ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_hastag, 0, 0, 1)
ZEND_ARG_INFO(0, tag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_istype, 0, 0, 1)
ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_at, 0, 0, 1)
ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_keyat, 0, 0, 1)
ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_valueat, 0, 0, 1)
ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_getnth, 0, 0, 1)
ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_get, 0, 0, 1)
ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_haskey, 0, 0, 1)
ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_slice_binaryeq, 0, 0, 1)
ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

zend_function_entry slice_methods[] =
{
	PHP_ME(Slice, __construct, vpack_slice_const, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, value, vpack_arginfo_none, ZEND_ACC_PUBLIC)
//	PHP_ME(Slice, toPHP, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getFirstTag, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getTags, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, hasTag, vpack_slice_hastag, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, type, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, typeName, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, hash, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, hash32, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, hashSlow, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, normalizedHash, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, normalizedHash32, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, hashString, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, hashString32, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isType, vpack_slice_istype, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isNone, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isIllegal, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isNull, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isBoolean, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isTrue, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isFalse, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isArray, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isObject, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isDouble, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isUTCDate, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isExternal, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isMinKey, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isMaxKey, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isInt, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isUInt, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isSmallInt, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isBinary, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isBCD, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isCustom, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isTagged, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isInteger, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isNumber, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isSorted, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getBoolean, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getDouble, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, at, vpack_slice_at, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, length, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, keyAt, vpack_slice_keyat, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, valueAt, vpack_slice_valueat, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getNthValue, vpack_slice_getnth, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, get, vpack_slice_get, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, hasKey, vpack_slice_haskey, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isEmptyArray, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, isEmptyObject, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, translate, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getInt, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getUInt, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getSmallInt, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getNumber, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getUTCDate, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getString, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getStringLength, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getBinary, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, getBinaryLength, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, byteSize, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, valueByteSize, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, binaryEquals, vpack_slice_binaryeq, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, toHex, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, toJson, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, toString, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Slice, __toString, vpack_arginfo_none, ZEND_ACC_PUBLIC)

#if PHP_VERSION_ID >= 80000
	PHP_ME(Slice, getIterator, vpack_arginfo_none, ZEND_ACC_PUBLIC)
#endif

	PHP_FE_END
};

void slice_init_ce(INIT_FUNC_ARGS)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "VPack\\Slice", slice_methods);
	slice_ce = zend_register_internal_class(&ce);
	slice_ce->create_object = slice_create_object;
	slice_ce->get_iterator = slice_get_iterator;

#if PHP_VERSION_ID >= 80000
	zend_class_implements(slice_ce, 1, zend_ce_aggregate);
#else
	zend_class_implements(slice_ce, 1, zend_ce_traversable);
#endif

	memcpy(&slice_handler, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	slice_handler.free_obj = slice_free_object;
	slice_handler.offset   = XtOffsetOf(php_vpack_slice_t, std);
}
