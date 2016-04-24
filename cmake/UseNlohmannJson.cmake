
if(NOT NlohmannJson_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann_json")
  add_subdirectory("third_party/nlohmann_json")
  set(NLOHMANN_JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann_json/src")
  set(NlohmannJson_FOUND TRUE)
endif()


