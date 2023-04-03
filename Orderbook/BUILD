cc_library(
  name = "include",
  hdrs = glob(["include/*.hpp"]),
  srcs = glob(["src/*.cpp"]),
  deps = ["@com_github_google_glog//:glog"]
)

cc_binary(
  name = "test_limit_tree",
  srcs = glob(["test/limit_tree.cpp"]),
  deps = ["@com_github_google_glog//:glog", ":include"]
)
