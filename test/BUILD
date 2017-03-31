cc_test(
    name = "cpplog_test",
    srcs = [
        "catch.hpp",
        "fakeit.hpp",
        "utils_test.cpp",
        "msgpack_test.cpp",
        "cpplog_unittest.cpp",
        "attachment_test.cpp",
        #"support_nlohmann_json_unittest.cpp"
    ],
    copts = [
        "-DHAVE_NLOHMANN_JSON"
#        "-std=c++14"
    ],
    deps = [
        "//cpplog:cpplog",
        "@fmtlib//:fmt",
        "@nlohmann_json//:json"
    ]
)

