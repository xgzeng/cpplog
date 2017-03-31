cc_test(
    name = "cpplog_test",
    srcs = [
        "catch.hpp",
        "fakeit.hpp",
        "cpplog_unittest.cpp",
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

