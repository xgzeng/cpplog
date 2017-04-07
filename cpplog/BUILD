
cc_library(
    name = "cpplog",
    hdrs = [
        "hex.h",
        "logging.h",
        "sink.h",
        "sinks/console_sink.h",
        "sinks/glog_sink.h",
        "sinks/udp_sink.h",
    ],
    srcs = [
        "config.h",
        "attachment.h",
        "record.h",
        "logging-inl.h",
        "sinks/console_sink_unix.h",
        "sinks/console_sink_windows.h",
        "sinks/udp_sink_inl.h"
    ],
    include_prefix = "cpplog",
    visibility = [
        "//visibility:public"
    ],
    deps = [
        "@nlohmann_json//:json",
        "@fmtlib//:fmt",
    ]
)

