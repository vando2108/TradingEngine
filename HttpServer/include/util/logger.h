// Copyright [2023] <Kdimo>
#ifndef __LOGGER_H__  // cpplint: NOLINT
#define __LOGGER_H__

#define PRINT(title, file, func, line, tick, fmt, ...) \
  printf("[%s] - %s:%s:%d %s, " fmt, title, file, func, line, tick, ##__VA_ARGS__) // cpplint: NOLINT

#define PRINT_INFO(title, tick, fmt, ...) \
  PRINT(title, __FILE__, __FUNCTION__, __LINE__, tick, fmt, ##__VA_ARGS__)

#define DEBUG

#ifdef DEBUG
  #define trace(fmt, ...) PRINT_INFO("trace", "", fmt, ##__VA_ARGS__)
  #define error(fmt, ...) PRINT_INFO("error", "", fmt, ##__VA_ARGS__)
  #define errsys(fmt, ...) PRINT_INFO("error", strerror(errno), fmt, ##__VA_ARGS__) // cpplint: NOLINT
#else
  #define trace(fmt, ...)
  #define error(fmt, ...)
  #define errsys(fmt, ...)
#endif

#endif // cpplint: NOLINT
