
cc_library(
  name = "include",
  hdrs = glob(["include/*.hpp"]),
  srcs = glob(["src/*.cpp"]),
)


cc_binary(
  name = "test_limit_tree",
  srcs = glob(["test/limit_tree.cpp"]),
  deps = [":include"]
)
