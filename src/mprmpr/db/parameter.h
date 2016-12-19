#ifndef MPRMPR_DB_PARAMETER_H_
#define MPRMPR_DB_PARAMETER_H_

#include <string>
#include <vector>
#include <cstring>

#include <mysql/mysql.h>

namespace mprmpr {
namespace db {

class Parameter : public MYSQL_BIND {
 public:
  Parameter(int8_t v) : Parameter(MYSQL_TYPE_TINY, v) {}
  Parameter(uint8_t v) : Parameter(MYSQL_TYPE_TINY, v) {}
  Parameter(int16_t v) : Parameter(MYSQL_TYPE_SHORT, v) {}
  Parameter(uint16_t v) : Parameter(MYSQL_TYPE_SHORT, v) {}
  Parameter(int32_t v) : Parameter(MYSQL_TYPE_LONG, v) {}
  Parameter(uint32_t v) : Parameter(MYSQL_TYPE_LONG, v) {}
  Parameter(int64_t v) : Parameter(MYSQL_TYPE_LONGLONG, v) {}
  Parameter(uint64_t v) : Parameter(MYSQL_TYPE_LONGLONG, v) {}
  Parameter(float v) : Parameter(MYSQL_TYPE_FLOAT, v) {}
  Parameter(double v) : Parameter(MYSQL_TYPE_DOUBLE, v) {}

  Parameter(const std::string& value) {
    ::memset(this, 0, sizeof(*this));
    buffer_type = MYSQL_TYPE_STRING;

    char* data = static_cast<char*>(std::malloc(value.size()));
    std::memcpy(data, value.c_str(), value.size());

    buffer = data;
    buffer_length = value.size();
  }

  Parameter(const std::vector<char>& value) {
    ::memset(this, 0, sizeof(*this));
    buffer_type = MYSQL_TYPE_BLOB;

    char* data = static_cast<char*>(std::malloc(value.size()));
    std::memcpy(data, value.data(), value.size());

    buffer = data;
    buffer_length = value.size();
  }  

  Parameter(std::nullptr_t value) {
    ::memset(this, 0, sizeof(*this));
    buffer_type = MYSQL_TYPE_NULL;
  }

  ~Parameter();

 private:
  template<typename T>
  Parameter(enum_field_types type, T value) {
    ::memset(this, 0, sizeof(*this));   
    buffer_type = type;
    is_unsigned = std::is_unsigned<T>::value;
    T* data = static_cast<T*>(std::malloc(sizeof(T)));
    *data = value;
    buffer = data;
  }
};

} // namespace db
} // namespace mprmpr
#endif // MPRMPR_DB_PARAMETER_H_
