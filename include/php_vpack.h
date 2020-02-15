#ifndef PHP_VPACK_H
#define PHP_VPACK_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {

#include "php.h"
#include <Zend/zend_interfaces.h>

}

#include <velocypack/vpack.h>
#include "vpack_aliases.h"
#include "vpack_classes.h"

#include "vpack_exceptions.h"
#include "Tagged.h"
#include "Serializable.h"
#include "Serializer.h"
#include "Slice.h"
#include "TypeMapper.h"
#include "Builder.h"

extern zend_module_entry vpack_module_entry;
#define phpext_vpack_ptr &vpack_module_entry

#define PHP_VPACK_VERSION "0.1.0"

#if defined(ZTS) && defined(COMPILE_DL_VPACK)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

/*
 * C++ exception macros
 */

#define VPACK_TRY                                                            \
  try {

#define VPACK_CATCH                                                          \
  } catch (std::exception const& ex) {                                       \
      zend_throw_exception_ex(vpack_exception_ce, 0, ex.what());             \
      return;                                                                \
  } catch (...) {                                                            \
      zend_throw_exception_ex(vpack_exception_ce, 0, "Unknown VPack error"); \
      return;                                                                \
  }

#endif
