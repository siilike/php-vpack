#ifndef VPACK_ALIASES_H_
#define VPACK_ALIASES_H_

#include <velocypack/vpack.h>

using VpackValueLength = arangodb::velocypack::ValueLength;
using VpackArrayIterator = arangodb::velocypack::ArrayIterator;
using VpackObjectIterator = arangodb::velocypack::ObjectIterator;
using VpackBuilder = arangodb::velocypack::Builder;
using VpackObjectBuilder = arangodb::velocypack::ObjectBuilder;
using VpackArrayBuilder = arangodb::velocypack::ArrayBuilder;
using VpackBuilderNonDeleter = arangodb::velocypack::BuilderNonDeleter;
using VpackBuilderContainer = arangodb::velocypack::BuilderContainer;
using VpackCharBuffer = arangodb::velocypack::CharBuffer;
template<typename T> using VpackBuffer = arangodb::velocypack::Buffer<T>;
using VpackSink = arangodb::velocypack::Sink;
using VpackCharBufferSink = arangodb::velocypack::CharBufferSink;
using VpackStringSink = arangodb::velocypack::StringSink;
using VpackStringStreamSink = arangodb::velocypack::StringStreamSink;
using VpackCollection = arangodb::velocypack::Collection;
using VpackAttributeTranslator = arangodb::velocypack::AttributeTranslator;
using VpackDumper = arangodb::velocypack::Dumper;
using VpackException = arangodb::velocypack::Exception;
using VpackHexDump = arangodb::velocypack::HexDump;
using VpackOptions = arangodb::velocypack::Options;
using VpackCustomTypeHandler = arangodb::velocypack::CustomTypeHandler;
using VpackParser = arangodb::velocypack::Parser;
using VpackSlice = arangodb::velocypack::Slice;
using VpackSliceContainer = arangodb::velocypack::SliceContainer;
using VpackStringRef = arangodb::velocypack::StringRef;
using VpackUtf8Helper = arangodb::velocypack::Utf8Helper;
using VpackValidator = arangodb::velocypack::Validator;
using VpackValue = arangodb::velocypack::Value;
using VpackValuePair = arangodb::velocypack::ValuePair;
using VpackValueType = arangodb::velocypack::ValueType;
using VpackVersion = arangodb::velocypack::Version;
using VpackNormalizedCompare = arangodb::velocypack::NormalizedCompare;
using VpackSerializable = arangodb::velocypack::Serializable;
using VpackSerialize = arangodb::velocypack::Serialize;

#endif
