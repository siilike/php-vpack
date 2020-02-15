
# PHP VelocyPack extension

This extension adds support for the VelocyPack format to PHP. It supports most methods of Slice and Builder and also ships with a tags-based type mapper for more precise type or object document mapping.

Tested with PHP 7.2.

See `src/Slice.cpp` and `src/Builder.cpp` for supported methods and `examples/` for type mapper examples.

## Building

```
git submodule update --recursive

phpize
./configure
make
make install
```
