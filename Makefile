CC = g++
CFLAGS = -std=gnu++20 -Wall -pthread  -g -fsanitize=address

LIBDIR = lib
BINDIR = bin
OBJDIR = $(BINDIR)/obj
TARGET = $(BINDIR)/execute

INCLUDE += include/event
INCLUDE += include/http
INCLUDE += include/log
INCLUDE += include/queue
INCLUDE += include/socket
INCLUDE += include/thread
INCLUDE += include/util

SRCDIR += src
SRCDIR += src/event
SRCDIR += src/http
SRCDIR += src/log
SRCDIR += src/queue
SRCDIR += src/socket
SRCDIR += src/thread
SRCDIR += src/util

include all-rules.mk
