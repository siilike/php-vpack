dnl config.m4 for extension vpack

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([vpack],
dnl   [for vpack support],
dnl   [AS_HELP_STRING([--with-vpack],
dnl     [Include vpack support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([vpack],
  [whether to enable vpack support],
  [AS_HELP_STRING([--enable-vpack],
    [Enable vpack support])],
  [yes])

if test "$PHP_VPACK" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, VPACK_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-vpack -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/vpack.h"  # you most likely want to change this
  dnl if test -r $PHP_VPACK/$SEARCH_FOR; then # path given as parameter
  dnl   VPACK_DIR=$PHP_VPACK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for vpack files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       VPACK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$VPACK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the vpack distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-vpack -> add include path
  dnl PHP_ADD_INCLUDE($VPACK_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-vpack -> check for lib and symbol presence
  dnl LIBNAME=VPACK # you may want to change this
  dnl LIBSYMBOL=VPACK # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_VPACK_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your vpack library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $VPACK_DIR/$PHP_LIBDIR, VPACK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_VPACK_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your vpack library.])
  dnl ],[
  dnl   -L$VPACK_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(VPACK_SHARED_LIBADD)

  AC_DEFINE(HAVE_VPACK, 1, [ Have vpack support ])

  VELOCYPACK_LIBRARY_PATH="velocypack/build"
  VELOCYPACK_INCLUDE="velocypack/include"
  
  PHP_ADD_INCLUDE($VELOCYPACK_INCLUDE)
  PHP_ADD_INCLUDE(include)
  
  CXXFLAGS="-std=c++11"
  LDFLAGS="-L$VELOCYPACK_LIBRARY_PATH -lvelocypack"

  PHP_REQUIRE_CXX()
  PHP_SUBST(VPACK_SHARED_LIBADD)

  PHP_ADD_LIBRARY(stdc++, 1, VPACK_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(velocypack, $VELOCYPACK_LIBRARY_PATH, VELOCYPACK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(vpack, src/php_vpack.cpp src/Slice.cpp src/TypeMapper.cpp src/Builder.cpp, $ext_shared)
fi
