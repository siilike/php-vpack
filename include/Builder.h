#ifndef MAPPER_H_
#define MAPPER_H_

#include "php_vpack.h"

extern zend_class_entry* builder_ce;

typedef struct php_vpack_builder_t
{
	VPACK_ZEND_OBJECT_PRE
	VpackBuffer<uint8_t> *buffer;
	VpackBuilder *builder;
	zval mapper;
	VPACK_ZEND_OBJECT_POST
} php_vpack_builder_t;

#define Z_BUILDER_OBJ_P(zv) (php_builder_fetch_object(Z_OBJ_P(zv)))
#define Z_OBJ_BUILDER(zo) (php_builder_fetch_object(zo))

static inline php_vpack_builder_t* php_builder_fetch_object(zend_object* obj)
{
	return (php_vpack_builder_t*) ((char*) obj - XtOffsetOf(php_vpack_builder_t, std));
}

void builder_init_ce(INIT_FUNC_ARGS);

void vpack_builder_add_slice(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackSlice& value);
void vpack_builder_add_value(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackValue& value);
void vpack_builder_add_value_pair(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackValuePair& value);
void vpack_builder_add_serializable(VpackBuilder *b, char *name, size_t nameLen, uint64_t tag, const VpackSerialize& value);

class LambdaVpackSerializable : public VpackSerializable
{
	public:
		std::function<void(VpackBuilder&)> lambda;

		LambdaVpackSerializable(std::function<void(VpackBuilder&)> lambda) : lambda(lambda) {}

		using VpackSerializable::toVelocyPack;
		void toVelocyPack(VpackBuilder& b) const override
		{
			lambda(b);
		}
};

#endif
