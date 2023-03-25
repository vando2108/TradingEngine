cc_library(
  name = "util",
  srcs = ["src/util/util.cpp"],
  hdrs = glob(["include/util/*.h"])
)

cc_library(
  name = "queue",
  hdrs = glob(["include/queue/*.h"])
)

cc_library(
  name = "thread",
  srcs = glob(["src/thread/*.cpp"]),
  hdrs = glob(["include/thread/*.h"]),
  deps = [":util", ":queue"]
)

cc_library(
  name = "socket",
  srcs = glob(["src/socket/*.cpp"]),
  hdrs = glob(["include/socket/*.h"]),
  deps = [":util"]
)

cc_library(
  name = "event",
  srcs = glob(["src/event/*.cpp"]),
  hdrs = glob(["include/event/*.hpp"]),
  deps = [":util", ":thread", ":socket"]
)

cc_library(
  name = "http",
  srcs = glob(["src/http/*.cpp"]),
  hdrs = glob(["include/http/*.h"]),
  deps = [":util", ":event", ":socket"]
)

cc_binary(
  name = "main",
  srcs = ["src/main.cpp"],
  deps = [":util", ":http", "@com_github_google_glog//:glog"]
)
