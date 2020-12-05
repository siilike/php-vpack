
# this is only for IDEA

cmake_minimum_required(VERSION 3.17)
project(vpack)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Debug)

list(PREPEND CMAKE_PROGRAM_PATH  "../php/bin/")
get_filename_component(PHP_SOURCE "../php-src" ABSOLUTE)

include_directories(.)
include_directories(include)
include_directories(velocypack/include)
include_directories(velocypack/include/velocypack)

include_directories("${PHP_SOURCE}")
include_directories("${PHP_SOURCE}/ext")
include_directories("${PHP_SOURCE}/main")
include_directories("${PHP_SOURCE}/sapi")
include_directories("${PHP_SOURCE}/TSRM")
include_directories("${PHP_SOURCE}/Zend")

add_executable(vpack
	include/Builder.h
	include/php_vpack.h
	include/Serializable.h
	include/Serializer.h
	include/Slice.h
	include/Tagged.h
	include/TypeMapper.h
	include/vpack_aliases.h
	include/vpack_classes.h
	include/vpack_exceptions.h
	src/Builder.cpp
	src/php_vpack.cpp
	src/Slice.cpp
	src/TypeMapper.cpp
	config.h
)
