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

  json to_json() const {
    return json_value_;
  }

private:
  json json_value_;
};

} // namespace cpplog

