load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "com_github_google_glog",
    sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
    strip_prefix = "glog-0.6.0",
    urls = ["https://github.com/google/glog/archive/v0.6.0.zip"],
)

http_archive(
    name = "cpplint_archive",
    build_file = "@//:cpplint.BUILD",
    sha256 = "b2979ff630299293f23c52096e408f2b359e2e26cb5cdf24aed4ce53e4293468",
    strip_prefix = "cpplint-1.2.2",
    url = "https://pypi.python.org/packages/source/c/cpplint/cpplint-1.2.2.tar.gz",
)
