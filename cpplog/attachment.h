#pragma once

#include "cpplog/config.h"

#include "json.hpp"  // nlohmann::json

namespace cpplog {

using json = nlohmann::json;

/*!
 *
 */
class JsonAttachment {
public:
  JsonAttachment()
  : json_value_(json::object()) {
  }

  // add integer property
  template<typename T>
  void add(string_view name, T&& value) {
    json_value_[std::string(name)] = value;
  }

private:
  json json_value_;
};

} // namespace cpplog

