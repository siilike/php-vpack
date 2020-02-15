
#include "php_vpack.h"
#include "Builder.h"
#include "TypeMapper.h"
#include "vpack_classes.h"


static zend_object_handlers builder_handler;


void vpack_builder_add_slice(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackSlice& value)
{
	if(name != nullptr)
	{
		b->addTagged(name, nameLen, tag, value);
	}
	else
	{
		b->addTagged(tag, value);
	}
}

void vpack_builder_add_value(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackValue& value)
{
	if(name != nullptr)
	{
		b->addTagged(name, nameLen, tag, value);
	}
	else
	{
		b->addTagged(tag, value);
	}
}

void vpack_builder_add_value_pair(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackValuePair& value)
{
	if(name != nullptr)
	{
		b->addTagged(name, nameLen, tag, value);
	}
	else
	{
		b->addTagged(tag, value);
	}
}

void vpack_builder_add_serializable(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackSerialize& value)
{
	if(name != nullptr)
	{
		b->addTagged(name, nameLen, tag, value);
	}
	else
	{
		b->addTagged(tag, value);
	}
}


PHP_METHOD(Builder, __construct)
{
	VPACK_TRY

	php_vpack_builder_t* intern;
	zval*                mapper;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &mapper) == FAILURE)
	{
		zend_throw_exception_ex(vpack_exception_ce, 0, "Type mapper is mandatory");
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	ZVAL_COPY(&intern->mapper, mapper);

	VPACK_CATCH
}

PHP_METHOD(Builder, toString)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	intern = Z_BUILDER_OBJ_P(getThis());

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	VpackSlice s = intern->builder->slice();

	std::string v = s.toString();

	RETURN_NEW_STR(zend_string_init(v.c_str(), v.length(), 0))

	VPACK_CATCH
}

PHP_METHOD(Builder, toJson)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	intern = Z_BUILDER_OBJ_P(getThis());

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	VpackSlice s = intern->builder->slice();

	std::string v = s.toJson();

	RETURN_NEW_STR(zend_string_init(v.c_str(), v.length(), 0))

	VPACK_CATCH
}

PHP_METHOD(Builder, reserve)
{
	VPACK_TRY

	php_vpack_builder_t* intern;
	zend_long            len;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &len) == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	intern->builder->reserve(len);

	VPACK_CATCH
}

PHP_METHOD(Builder, clear)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	intern->builder->clear();

	VPACK_CATCH
}

PHP_METHOD(Builder, slice)
{
	VPACK_TRY

	php_vpack_builder_t* intern;
	zval               ret;
	zval               str;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	VpackSlice s = intern->builder->slice();

	ZVAL_STRINGL(&str, (char*) s.start(), s.byteSize()); // TODO: zero-copy

	new_slice_ref(&ret, &str, 0);

	zval_ptr_dtor(&str);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

PHP_METHOD(Builder, size)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	RETURN_LONG(intern->builder->size());

	VPACK_CATCH
}

PHP_METHOD(Builder, isEmpty)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	RETURN_BOOL(intern->builder->isEmpty());

	VPACK_CATCH
}

PHP_METHOD(Builder, isClosed)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	RETURN_BOOL(intern->builder->isClosed());

	VPACK_CATCH
}

PHP_METHOD(Builder, isOpenArray)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	RETURN_BOOL(intern->builder->isOpenArray());

	VPACK_CATCH
}

PHP_METHOD(Builder, isOpenObject)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	RETURN_BOOL(intern->builder->isOpenObject());

	VPACK_CATCH
}

PHP_METHOD(Builder, openArray)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE) // TODO unindexed
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	intern->builder->openArray();

	VPACK_CATCH
}

PHP_METHOD(Builder, openObject)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE) // TODO unindexed
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	intern->builder->openObject();

	VPACK_CATCH
}

PHP_METHOD(Builder, addObjectValue)
{
	VPACK_TRY

	php_vpack_builder_t* intern;
	char*                key;
	size_t               keyLen;
	zend_long            tag = 0;
	zval*                input;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "slz", &key, &keyLen, &tag, &input) == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	LambdaVpackSerializable ls([&](VpackBuilder &_unused)
	{
		serialize(nullptr, 0, 0, &intern->mapper, intern->builder, input);
	});

	intern->builder->addTagged(key, keyLen, tag, VpackSerialize(ls));

	VPACK_CATCH
}

PHP_METHOD(Builder, addValue)
{
	VPACK_TRY

	php_vpack_builder_t* intern;
	zend_long            tag = 0;
	zval*                input;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &tag, &input) == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	LambdaVpackSerializable ls([&](VpackBuilder &_unused)
	{
		serialize(nullptr, 0, 0, &intern->mapper, intern->builder, input);
	});

	intern->builder->addTagged(tag, VpackSerialize(ls));

	VPACK_CATCH
}

PHP_METHOD(Builder, close)
{
	VPACK_TRY

	php_vpack_builder_t* intern;

	if(zend_parse_parameters_none() == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	intern->builder->close();

	VPACK_CATCH
}

PHP_METHOD(Builder, hasKey)
{
	VPACK_TRY

	php_vpack_builder_t* intern;
	char*              key;
	size_t             keyLen = 0;
	zval               ret;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keyLen) == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	RETURN_BOOL(intern->builder->hasKey(std::string(key, keyLen)));

	VPACK_CATCH
}

PHP_METHOD(Builder, getKey)
{
	VPACK_TRY

	php_vpack_builder_t* intern;
	char*              key;
	size_t             keyLen = 0;
	zval               ret;
	zval               str;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keyLen) == FAILURE)
	{
		return;
	}

	intern = Z_BUILDER_OBJ_P(getThis());

	VpackSlice s = intern->builder->getKey(std::string(key, keyLen));

	ZVAL_STRINGL(&str, (char*) s.start(), s.byteSize()); // TODO: zero-copy

	new_slice_ref(&ret, &str, 0);

	zval_ptr_dtor(&str);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}


static zend_object* builder_create_object(zend_class_entry* class_type)
{
	php_vpack_builder_t* intern = NULL;

	intern = (php_vpack_builder_t*) ecalloc(1, sizeof(php_vpack_builder_t) + zend_object_properties_size(class_type));
//	intern = zend_object_alloc(sizeof(php_vpack_builder_t), php_vpack_builder_t);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->std.handlers = &builder_handler;

	intern->buffer = new VpackBuffer<uint8_t>(250);
	intern->builder = new VpackBuilder(*intern->buffer);

	return &intern->std;
}

static void builder_free_object(zend_object* object)
{
	php_vpack_builder_t* intern = Z_OBJ_BUILDER(object);

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	delete intern->buffer;
	delete intern->builder;

	zval_ptr_dtor(&intern->mapper);
}

ZEND_BEGIN_ARG_INFO_EX(vpack_builder_const, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_builder_reserve, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_builder_addobjval, 0, 0, 3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_builder_addvalue, 0, 0, 2)
ZEND_END_ARG_INFO()

zend_function_entry builder_methods[] =
{
	PHP_ME(Builder, __construct, vpack_builder_const, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, reserve, vpack_builder_reserve, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, clear, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, size, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, isEmpty, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, isClosed, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, isOpenArray, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, isOpenObject, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, openArray, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, openObject, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, addObjectValue, vpack_builder_addobjval, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, addValue, vpack_builder_addvalue, ZEND_ACC_PUBLIC)
	PHP_ME(Builder, close, vpack_arginfo_none, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void builder_init_ce(INIT_FUNC_ARGS)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "VPack\\Builder", builder_methods);
	builder_ce = zend_register_internal_class(&ce TSRMLS_CC);
	builder_ce->create_object = builder_create_object;

	memcpy(&builder_handler, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	builder_handler.free_obj = builder_free_object;
	builder_handler.offset   = XtOffsetOf(php_vpack_builder_t, std);
}
