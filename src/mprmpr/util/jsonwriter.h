#ifndef KUDU_UTIL_JSONWRITER_H
#define KUDU_UTIL_JSONWRITER_H

#include <inttypes.h>

#include <memory>
#include <string>

#include "mprmpr/base/macros.h"

namespace google {
namespace protobuf {
class Message;
class FieldDescriptor;
} // namespace protobuf
} // namespace google

namespace mprmpr {

class JsonWriterIf;

// Acts as a pimpl for rapidjson so that not all metrics users must bring in the
// rapidjson library, which is template-based and therefore hard to forward-declare.
//
// This class implements all the methods of rapidjson::JsonWriter, plus an
// additional convenience method for String(std::string).
//
// We take an instance of std::stringstream in the constructor because Mongoose / Squeasel
// uses std::stringstream for output buffering.
class JsonWriter {
 public:
  enum Mode {
    // Pretty-print the JSON, with nice indentation, newlines, etc.
    PRETTY,
    // Print the JSON as compactly as possible.
    COMPACT
  };

  JsonWriter(std::ostringstream* out, Mode mode);
  ~JsonWriter();

  void Null();
  void Bool(bool b);
  void Int(int i);
  void Uint(unsigned u);
  void Int64(int64_t i64);
  void Uint64(uint64_t u64);
  void Double(double d);
  void String(const char* str, size_t length);
  void String(const char* str);
  void String(const std::string& str);

  void Protobuf(const google::protobuf::Message& message);

  template<typename T>
  void Value(const T& val);

  void StartObject();
  void EndObject();
  void StartArray();
  void EndArray();

  // Convert the given protobuf to JSON format.
  static std::string ToJson(const google::protobuf::Message& pb,
                            Mode mode);

 private:
  void ProtobufField(const google::protobuf::Message& pb,
                     const google::protobuf::FieldDescriptor* field);
  void ProtobufRepeatedField(const google::protobuf::Message& pb,
                             const google::protobuf::FieldDescriptor* field,
                             int index);

  std::unique_ptr<JsonWriterIf> impl_;
  DISALLOW_COPY_AND_ASSIGN(JsonWriter);
};

} // namespace mprmpr

#endif // KUDU_UTIL_JSONWRITER_H
