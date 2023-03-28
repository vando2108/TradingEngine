cc_library(
  name = "util",
  hdrs = glob(["include/util/*.hpp"]),
)

cc_library(
  name = "include",
  hdrs = glob(["include/*.hpp", "include/order/*.hpp"]),
  srcs = glob(["src/*.cpp"]),
  deps = [":util", "@com_github_google_glog//:glog"]
)

cc_binary(
  name = "main",
  srcs = ["src/main.cpp"],
  deps = [":include", "@com_github_google_glog//:glog"],
)

cc_binary(
  name = "test",
  srcs = glob(["test/*.cpp"]),
  deps = [":include"]
)
