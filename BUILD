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
