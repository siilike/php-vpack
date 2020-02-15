
extern "C" {

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

}

#include "php_vpack.h"


zend_class_entry* slice_ce;
zend_class_entry* vpack_tagged_ce;
zend_class_entry* vpack_serializer_ce;
zend_class_entry* vpack_serializable_ce;
zend_class_entry* mapper_ce;
zend_class_entry* builder_ce;


PHP_RINIT_FUNCTION(vpack)
{
#if defined(ZTS) && defined(COMPILE_DL_VPACK)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(vpack)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "vpack support", "enabled");
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(vpack)
{
	slice_init_ce(INIT_FUNC_ARGS_PASSTHRU);
	mapper_init_ce(INIT_FUNC_ARGS_PASSTHRU);
	builder_init_ce(INIT_FUNC_ARGS_PASSTHRU);

	vpack_register_exception(INIT_FUNC_ARGS_PASSTHRU);
	vpack_register_interface_tagged(INIT_FUNC_ARGS_PASSTHRU);
	vpack_register_interface_serializable(INIT_FUNC_ARGS_PASSTHRU);
	vpack_register_interface_serializer(INIT_FUNC_ARGS_PASSTHRU);

	REGISTER_STRING_CONSTANT("VPACK_VERSION", (char*) "0.1.0", CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

static const zend_function_entry vpack_functions[] =
{
	PHP_FE_END
};

zend_module_entry vpack_module_entry =
{
	STANDARD_MODULE_HEADER,
	"vpack",					/* Extension name */
	vpack_functions,			/* zend_function_entry */
	PHP_MINIT(vpack),			/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(vpack),			/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(vpack),			/* PHP_MINFO - Module info */
	PHP_VPACK_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_VPACK
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(vpack)
#endif
