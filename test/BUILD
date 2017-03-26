cc_test(
    name = "cpplog_test",
    srcs = [
        "catch.hpp",
        "fakeit.hpp",
        "utils_test.cpp",
        "msgpack_test.cpp",
        #"cpplog_unittest.cpp",
        #"support_nlohmann_json_unittest.cpp"
    ],
#    copts = [
#        "-std=c++14"
#    ],
    deps = [
        "//cpplog:cpplog",
        "@fmtlib//:fmt",
        "@nlohmann_json//:json"
    ]
)

