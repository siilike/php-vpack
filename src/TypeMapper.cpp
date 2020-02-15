
#include "php_vpack.h"
#include "TypeMapper.h"
#include "Serializable.h"
#include "vpack_classes.h"


static zend_object_handlers mapper_handler;


void serialize(char* name, size_t nameLen, uint64_t tag, zval* zmapper, VpackBuilder *b, zval* input);

void serializeHashTable(char* name, size_t nameLen, uint64_t tag, zval* zmapper, VpackBuilder *b, HashTable* input)
{
	zend_string *key;
	zend_long key_index;
	zval *val;
	HashPosition pos;

	if(name != NULL || nameLen != 0 || tag != 0)
	{
		// TODO
		zend_error(E_ERROR, "serializeHashTable does not support adding keys and tags");
	}

	bool isObject = zend_hash_get_current_key_type(input) != HASH_KEY_IS_LONG;

	if(!isObject)
	{
		zend_hash_internal_pointer_reset_ex(input, &pos);

		while(zend_hash_move_forward_ex(input, &pos) == SUCCESS)
		{
			if(zend_hash_get_current_key_type_ex(input, &pos) == HASH_KEY_IS_STRING)
			{
				isObject = true;
				break;
			}
		}
	}

	if(isObject)
	{
		b->openObject();

		ZEND_HASH_FOREACH_KEY_VAL(input, key_index, key, val)
		{
			if(key)
			{
				serialize(ZSTR_VAL(key), ZSTR_LEN(key), 0, zmapper, b, val);
			}
			else
			{
				int len = snprintf(NULL, 0, "%d", key_index);
				char k[len+1];
				snprintf(k, len+1, "%d", key_index);

				serialize(k, len, 0, zmapper, b, val);
			}
		} ZEND_HASH_FOREACH_END();
	}
	else
	{
		b->openArray();

		ZEND_HASH_FOREACH_VAL(input, val)
		{
			serialize(nullptr, 0, 0, zmapper, b, val);
		} ZEND_HASH_FOREACH_END();
	}

	b->close();
}

void serialize(char* name, size_t nameLen, uint64_t tag, zval* zmapper, VpackBuilder *b, zval* input0)
{
	php_vpack_mapper_t *intern = Z_MAPPER_OBJ_P(zmapper);
	zval* input = input0;

	if(Z_TYPE_P(input) == IS_INDIRECT)
	{
		input = Z_INDIRECT_P(input);
	}

	if(Z_ISREF_P(input))
	{
		ZVAL_DEREF(input);
	}

	zend_uchar type = Z_TYPE_P(input);

	switch(type)
	{
		case IS_NULL:
			vpack_builder_add_slice(b, name, nameLen, tag, VpackSlice::nullSlice());
			break;
		case IS_FALSE:
			vpack_builder_add_slice(b, name, nameLen, tag, VpackSlice::falseSlice());
			break;
		case IS_TRUE:
			vpack_builder_add_slice(b, name, nameLen, tag, VpackSlice::trueSlice());
			break;
		case IS_LONG:
			vpack_builder_add_value(b, name, nameLen, tag, VpackValue(static_cast<uint64_t>(zval_get_long(input))));
			break;
		case IS_DOUBLE:
			vpack_builder_add_value(b, name, nameLen, tag, VpackValue(static_cast<double_t>(zval_get_double(input))));
			break;
		case IS_STRING:
			vpack_builder_add_value_pair(b, name, nameLen, tag, VpackValuePair(ZSTR_VAL(Z_STR_P(input)), ZSTR_LEN(Z_STR_P(input)), VpackValueType::String));
			break;
		case IS_OBJECT:
		{
			zend_long otag = 0;

			if(instanceof_function(Z_OBJCE_P(input), vpack_serializable_ce))
			{
				zval ret;
				zval tagret;

				zend_call_method_with_0_params(input, NULL, NULL, "getTagId", &tagret);

				if(Z_ISUNDEF(tagret))
				{
					return;
				}

				otag = Z_LVAL(tagret);

				LambdaVpackSerializable ls([&](VpackBuilder& _unused)
				{
					zend_call_method_with_2_params(input, NULL, NULL, "toVPack", &ret, &intern->builder, zmapper);

					if(Z_ISUNDEF(ret) || Z_ISUNDEF(tagret))
					{
						return;
					}
				});

				vpack_builder_add_serializable(b, name, nameLen, otag, VpackSerialize(ls));

				zval_ptr_dtor(&ret);
				zval_ptr_dtor(&tagret);
			}
			else
			{
				zval* tag = zend_hash_find(Z_ARR(intern->classToTagMap), Z_OBJCE_P(input)->name);
				zval* serializer = NULL;

				if(tag)
				{
					serializer = zend_hash_index_find(Z_ARR(intern->serializers), Z_LVAL_P(tag));

					otag = Z_LVAL_P(tag);
				}

				LambdaVpackSerializable ls([&](VpackBuilder& _unused)
				{
					if(serializer)
					{
						zval ret;
						zval args[3];

						ZVAL_COPY(&args[0], input);
						ZVAL_COPY(&args[1], &intern->builder);
						ZVAL_COPY(&args[2], zmapper);

						if(call_user_function(NULL, NULL, serializer, &ret, 3, args) != SUCCESS || Z_ISUNDEF(ret))
						{
							// failed
						}

						zval_ptr_dtor(&args[0]);
						zval_ptr_dtor(&args[1]);
						zval_ptr_dtor(&args[2]);
						zval_ptr_dtor(&ret);
					}
					else
					{
						// TODO: __sleep()

						php_error_docref(NULL, E_WARNING, "No serializer for class '%s' found, writing public keys", ZSTR_VAL(Z_OBJCE_P(input)->name));

						serializeHashTable(nullptr, 0, 0, zmapper, b, Z_OBJ_HT_P(input)->get_properties(input));
					}
				});

				vpack_builder_add_serializable(b, name, nameLen, otag, VpackSerialize(ls));
			}
			break;
		}
		case IS_ARRAY:
		{
			LambdaVpackSerializable ls([&](VpackBuilder& _unused)
			{
				serializeHashTable(nullptr, 0, 0, zmapper, b, Z_ARRVAL_P(input));
			});

			vpack_builder_add_serializable(b, name, nameLen, tag, VpackSerialize(ls));
			break;
		}
		default:
			php_error_docref(NULL, E_WARNING, "Unsupported type %d", type);
			break;
	}
}

void deserialize(zval* ret, zval* zs, const VpackSlice &s, zval* zmapper)
{
	php_vpack_mapper_t* intern = Z_MAPPER_OBJ_P(zmapper);

	switch(s.type())
	{
		case VpackValueType::Int:
		case VpackValueType::UInt:
		case VpackValueType::SmallInt:
			ZVAL_LONG(ret, s.getInt());
			break;
		case VpackValueType::String:
		{
			VpackValueLength length;
			char *str = (char*) s.getString(length);
			ZVAL_STRINGL(ret, str, length);
			break;
		}
		case VpackValueType::Bool:
			ZVAL_BOOL(ret, s.getBoolean());
			break;
		case VpackValueType::Null:
		case VpackValueType::None:
			ZVAL_NULL(ret);
			break;
		case VpackValueType::BCD:
		{
			ZVAL_NULL(ret); // TODO
			break;
		}
		case VpackValueType::Object:
		{
			array_init_size(ret, s.length());

			for(auto const& it : VpackObjectIterator(s))
			{
				zval v;
				const char* key;
				VpackValueLength keyLen;

				key = it.key.getString(keyLen);

				deserialize(&v, zs, it.value, zmapper);

				add_assoc_zval_ex(ret, key, keyLen, &v);
			}
			break;
		}
		case VpackValueType::Array:
		{
			array_init_size(ret, s.length());

			for(auto const& it : VpackArrayIterator(s))
			{
				zval v;

				deserialize(&v, zs, it, zmapper);

				add_next_index_zval(ret, &v);
			}
			break;
		}
		case VpackValueType::Tagged:
		{
			zval* deserializer;
			zval* cls;

			deserializer = zend_hash_index_find(Z_ARR(intern->deserializers), s.getFirstTag());

			if(deserializer)
			{
				zval slice;
				zval args[2];

				VpackSlice sv = s.value();

				new_slice_ref_zval(&slice, zs, sv);

				ZVAL_COPY(&args[0], &slice);
				ZVAL_COPY(&args[1], zmapper);

				if(call_user_function(NULL, NULL, deserializer, ret, 2, args) != SUCCESS || Z_ISUNDEF_P(ret))
				{
					// failed
				}

				zval_ptr_dtor(&args[0]);
				zval_ptr_dtor(&args[1]);
				zval_ptr_dtor(&slice);
				return;
			}

			cls = zend_hash_index_find(Z_ARR(intern->tagToClassMap), s.getFirstTag());

			if(cls)
			{
				zval slice;
				zend_class_entry* ce = zend_lookup_class(Z_STR_P(cls));

				if(!ce)
				{
					zend_throw_exception_ex(vpack_exception_ce, 0, "Class '%s' not found", ZSTR_VAL(Z_STR_P(cls)));
					return;
				}

				if(!instanceof_function(ce, vpack_serializable_ce))
				{
					zend_throw_exception_ex(vpack_exception_ce, 0, "Class '%s' not Serializable", ZSTR_VAL(Z_STR_P(cls)));
					return;
				}

				VpackSlice sv = s.value();

				new_slice_ref_zval(&slice, zs, sv);

				zend_call_method(NULL, ce, NULL, "fromvpack", strlen("fromvpack"), ret, 2, &slice, zmapper);

				if(Z_ISUNDEF_P(ret))
				{
					zend_throw_exception_ex(vpack_exception_ce, 0, "Deserializing '%s' failed", ZSTR_VAL(Z_STR_P(cls)));
					return;
				}

				zval_ptr_dtor(&slice);
				return;
			}

			php_error_docref(NULL, E_WARNING, "Unsupported tag '%s'", s.getFirstTag());
			ZVAL_NULL(ret);
			break;
		}
		default:
			php_error_docref(NULL, E_WARNING, "Unsupported VPack type '%s'", s.typeName());
			ZVAL_NULL(ret);
			break;
	}
}



PHP_METHOD(TypeMapper, __construct)
{
	VPACK_TRY

	php_vpack_mapper_t* intern;
	zval*               tagToClassMap;
	zval*               classToTagMap;
	zval*               serializers;
	zval*               deserializers;

	// TODO
	if(zend_parse_parameters(ZEND_NUM_ARGS(), "|zzzz", &tagToClassMap, &classToTagMap, &serializers, &deserializers) == FAILURE)
	{
		return;
	}

	intern = Z_MAPPER_OBJ_P(getThis());

	array_init_size(&intern->tagToClassMap, 25);
	array_init_size(&intern->classToTagMap, 25);
	array_init_size(&intern->serializers, 25);
	array_init_size(&intern->deserializers, 25);

	zend_update_property(mapper_ce, getThis(), "tagToClassMap", sizeof("tagToClassMap")-1, &intern->tagToClassMap);
	zend_update_property(mapper_ce, getThis(), "classToTagMap", sizeof("classToTagMap")-1, &intern->classToTagMap);
	zend_update_property(mapper_ce, getThis(), "serializers", sizeof("serializers")-1, &intern->serializers);
	zend_update_property(mapper_ce, getThis(), "deserializers", sizeof("deserializers")-1, &intern->deserializers);

	object_init_ex(&intern->builder, builder_ce);

	zval fname;
	zval retval;
	zval params[1];
	int result;

	ZVAL_STRING(&fname, "__construct");
	ZVAL_COPY(&params[0], getThis());

	result = call_user_function_ex(NULL, &intern->builder, &fname, &retval, 1, params, 0, NULL);

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&fname);
	zval_ptr_dtor(&params[0]);

	if(result != SUCCESS)
	{
		zend_error(E_ERROR, "Failed to create Builder object");
	}

	VPACK_CATCH
}

PHP_METHOD(TypeMapper, registerMapping)
{
	VPACK_TRY

	php_vpack_mapper_t* intern;
	zend_long           tag;
	char*               className;
	size_t              classNameLen;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &tag, &className, &classNameLen) == FAILURE)
	{
		return;
	}

	intern = Z_MAPPER_OBJ_P(getThis());

	add_index_stringl(&intern->tagToClassMap, tag, className, classNameLen);
	add_assoc_long_ex(&intern->classToTagMap, className, classNameLen, tag);

	VPACK_CATCH
}

PHP_METHOD(TypeMapper, registerSerializer)
{
	VPACK_TRY

	php_vpack_mapper_t* intern;
	zend_long           tag;
	zval*               callable;
	zval                copy;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &tag, &callable) == FAILURE)
	{
		return;
	}

	intern = Z_MAPPER_OBJ_P(getThis());

	if(!zend_is_callable(callable, IS_CALLABLE_CHECK_NO_ACCESS, NULL))
	{
		zend_throw_exception_ex(vpack_exception_ce, 0, "Second argument needs to be a valid callback");
	}

	ZVAL_COPY(&copy, callable);

	add_index_zval(&intern->serializers, tag, &copy);

	VPACK_CATCH
}

PHP_METHOD(TypeMapper, registerDeserializer)
{
	VPACK_TRY

	php_vpack_mapper_t* intern;
	zend_long           tag;
	zval*               callable;
	zval                copy;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &tag, &callable) == FAILURE)
	{
		return;
	}

	intern = Z_MAPPER_OBJ_P(getThis());

	if(!zend_is_callable(callable, IS_CALLABLE_CHECK_NO_ACCESS, NULL))
	{
		zend_throw_exception_ex(vpack_exception_ce, 0, "Second argument needs to be a valid callback");
	}

	ZVAL_COPY(&copy, callable);

	add_index_zval(&intern->deserializers, tag, &copy);

	VPACK_CATCH
}

PHP_METHOD(TypeMapper, serialize)
{
	VPACK_TRY

	php_vpack_mapper_t*  intern;
	php_vpack_builder_t* builder;
	zval*                input;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &input) == FAILURE)
	{
		return;
	}

	intern = Z_MAPPER_OBJ_P(getThis());
	builder = Z_BUILDER_OBJ_P(&intern->builder);

	builder->builder->clear();

	serialize(nullptr, 0, 0, getThis(), builder->builder, input);

	VpackSlice s = builder->builder->slice();

	RETURN_NEW_STR(zend_string_init((const char*) s.start(), s.byteSize(), 0))

	VPACK_CATCH
}

PHP_METHOD(TypeMapper, deserialize)
{
	VPACK_TRY

	php_vpack_mapper_t* intern;
	zval*               input;
	zval                copy;
	zval                ret;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &input) == FAILURE)
	{
		return;
	}

	intern = Z_MAPPER_OBJ_P(getThis());

	ZVAL_COPY(&copy, input);

	VpackSlice s = VpackSlice((const uint8_t*) ZSTR_VAL(Z_STR(copy)));

	deserialize(&ret, &copy, s, getThis());

	zval_ptr_dtor(&copy);

	RETURN_ZVAL(&ret, 0, 1);

	VPACK_CATCH
}

static zend_object* mapper_create_object(zend_class_entry* class_type)
{
	php_vpack_mapper_t* intern = NULL;

	intern = (php_vpack_mapper_t*) ecalloc(1, sizeof(php_vpack_mapper_t) + zend_object_properties_size(class_type));
//	intern = zend_object_alloc(sizeof(php_vpack_mapper_t), php_vpack_mapper_t);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->std.handlers = &mapper_handler;

	return &intern->std;
}

static void mapper_free_object(zend_object* object)
{
	php_vpack_mapper_t* intern = Z_OBJ_MAPPER(object);

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	zval_ptr_dtor(&intern->tagToClassMap);
	zval_ptr_dtor(&intern->classToTagMap);
	zval_ptr_dtor(&intern->serializers);
	zval_ptr_dtor(&intern->deserializers);
	zval_ptr_dtor(&intern->builder);
}

ZEND_BEGIN_ARG_INFO_EX(vpack_mapper_const, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_mapper_register_mapping, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_mapper_register_serializer, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_mapper_register_deserializer, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_mapper_register_serialize, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vpack_mapper_register_deserialize, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry mapper_methods[] =
{
	PHP_ME(TypeMapper, __construct, vpack_mapper_const, ZEND_ACC_PUBLIC)
	PHP_ME(TypeMapper, registerMapping, vpack_mapper_register_mapping, ZEND_ACC_PUBLIC)
	PHP_ME(TypeMapper, registerSerializer, vpack_mapper_register_serializer, ZEND_ACC_PUBLIC)
	PHP_ME(TypeMapper, registerDeserializer, vpack_mapper_register_deserializer, ZEND_ACC_PUBLIC)
	PHP_ME(TypeMapper, serialize, vpack_mapper_register_serialize, ZEND_ACC_PUBLIC)
	PHP_ME(TypeMapper, deserialize, vpack_mapper_register_deserialize, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void mapper_init_ce(INIT_FUNC_ARGS)
{
	zend_class_entry ce;
	zval             tmp;

	INIT_CLASS_ENTRY(ce, "VPack\\TypeMapper", mapper_methods);
	mapper_ce = zend_register_internal_class(&ce TSRMLS_CC);
	mapper_ce->create_object = mapper_create_object;

	ZVAL_NULL(&tmp);

	zend_declare_property(mapper_ce, "tagToClassMap", sizeof("tagToClassMap")-1, &tmp, ZEND_ACC_PUBLIC);
	zend_declare_property(mapper_ce, "classToTagMap", sizeof("classToTagMap")-1, &tmp, ZEND_ACC_PUBLIC);
	zend_declare_property(mapper_ce, "serializers", sizeof("serializers")-1, &tmp, ZEND_ACC_PUBLIC);
	zend_declare_property(mapper_ce, "deserializers", sizeof("deserializers")-1, &tmp, ZEND_ACC_PUBLIC);

	memcpy(&mapper_handler, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	mapper_handler.free_obj = mapper_free_object;
	mapper_handler.offset   = XtOffsetOf(php_vpack_mapper_t, std);
}
