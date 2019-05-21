load("@envoy_api//bazel:api_build_system.bzl", "api_go_proto_library", "api_proto_library_internal")

licenses(["notice"])  # Apache 2

api_proto_library_internal(
    name = "matchers",
    srcs = ["matchers.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//envoy/api/v2/route",
    ],
)

api_go_proto_library(
    name = "matchers",
    proto = ":matchers",
    deps = [
        "//envoy/api/v2/route:route_go_proto",
    ],
)

api_proto_library_internal(
    name = "matcher_reference",
    srcs = ["matcher_reference.proto"],
    visibility = ["//visibility:public"],
)
