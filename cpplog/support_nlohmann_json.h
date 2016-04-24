#pragma once
#include <cpplog/json_builder.h>
#include <json.hpp>

namespace cpplog {

template<>
struct dumper<nlohmann::json> {
  void operator()(JsonBuilder& jb, const nlohmann::json& value) {
    typedef nlohmann::json::value_t value_t;
    switch(value.type()) {
    case value_t::null:
      jb.WriteNull();
      break;
    case value_t::object:
      {
        jb.ObjectBegin();
        auto& r = value.get_ref<const nlohmann::json::object_t&>();
        for (auto i = r.begin(); i != r.end(); ++i) {
          jb.WriteName(i->first);
          operator()(jb, i->second);
        }
        jb.ObjectEnd();
      }
       break;
    case value_t::array:
      {
        jb.ArrayBegin();
        auto& r = value.get_ref<const nlohmann::json::array_t&>();
        for (auto i = r.begin(); i != r.end(); ++i) {
          operator()(jb, *i);
        }
        jb.ArrayEnd();
      }
      break;
    case value_t::string:
      jb.WriteString(value.get<std::string>());
      break;
    case value_t::boolean:
      jb.WriteBool(value.get<bool>());
      break;
    case value_t::number_integer:
      jb.WriteInteger(value.get<int64_t>());
      break;
    case value_t::number_unsigned:
      jb.WriteInteger(value.get<uint64_t>());
      break;
    case value_t::number_float:
      jb.WriteDouble(value.get<double>());
      break;
    case value_t::discarded:
      assert(false);
    }
  }
};

};

