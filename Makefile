XX = g++
CXXFLAGS = -Wall -g -O2 -fPIC -fopenmp -Wextra -std=c++17 \
	-Iinclude \
	-I$(HOME)/local/include

TEST_CFLAGS = $(CXXFLAGS) -Itests/catch2

OBJDIR = obj
LIBDIR = lib
BINDIR = bin

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRC))
LIB = $(LIBDIR)/liblbm.a

TEST_SRC = $(wildcard tests/*.cpp)
TEST_OBJ = $(patsubst tests/%.cpp,$(OBJDIR)/test_%.o,$(filter %.cpp,$(TEST_SRC)))
CATCH_SRC = tests/catch2/catch_amalgamated.cpp
CATCH_OBJ = $(OBJDIR)/catch_amalgamated.o

.PHONY: all base sim tests test clean

all: base test_runner

$(OBJDIR) $(LIBDIR) $(BINDIR):
	mkdir -p $@

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB): $(OBJ) | $(LIBDIR)
	ar rcs $@ $^

base: $(LIB)

$(OBJDIR)/test_%.o: tests/%.cpp | $(OBJDIR)
	$(CXX) $(TEST_CFLAGS) -c $< -o $@

$(OBJDIR)/catch_amalgamated.o: $(CATCH_SRC) | $(OBJDIR)
	$(CXX) $(TEST_CFLAGS) -c $< -o $@

test_runner: $(TEST_OBJ) $(CATCH_OBJ) $(LIB)
	$(CXX) $(TEST_CFLAGS) -o $@ $(TEST_OBJ) $(CATCH_OBJ) -L$(LIBDIR) -llbm

# Convenience targets
tests: test_runner

test: test_runner
	./test_runner

CLEANABLE_OBJS = $(filter-out $(OBJDIR)/catch_amalgamated.o,$(wildcard $(OBJDIR)/*.o))

clean:
	rm -f $(CLEANABLE_OBJS) $(LIB) bin/pbalitesim test_runner

cleanall:
	rm -rf $(OBJDIR) $(LIB) bin/pbalitesim test_runner

.PHONY: all base sim tests test clean cleanall



# CC = g++
# CFLAGS = -Wall -g -O2 -fPIC -fopenmp -Wextra -std=c++17 \
# -Iinclude \
# -I$(HOME)/local/include \
# #-isystem /home/leftcircle/programming/include/ 

# TEST_CFLAGS = $(CFLAGS) -Itests/catch2

# LDFLAGS  := -L$(HOME)/local/lib64 -Wl,-rpath,$(HOME)/local/lib64
# LDLIBS = -lOpenImageIO -lOpenImageIO_Util -lGL -lGLU -lglut


# SRC = $(wildcard src/*.cpp)
# MAIN = main.cpp
# OBJDIR = obj
# OBJS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRC)) $(OBJDIR)/main.o

# # Test files
# TEST_SRC = $(wildcard tests/*.cpp)
# CATCH_SRC = tests/catch2/catch_amalgamated.cpp
# TEST_OBJS = $(patsubst tests/%.cpp,$(OBJDIR)/test_%.o,$(filter %.cpp,$(TEST_SRC))) \
# 			$(OBJDIR)/catch_amalgamated.o


# ROOTDIR = .
# LIB = $(ROOTDIR)/lib/liblbm.a
# #TARGET = imgviewer

# # all: $(OBJDIR) $(TARGET)

# # $(OBJDIR):
# # 	mkdir -p $(OBJDIR)

# # $(TARGET): $(OBJS)
# # 	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# # $(OBJDIR)/%.o: src/%.cpp
# # 	$(CC) $(CFLAGS) -c $< -o $@

# # $(OBJDIR)/main.o: main.cpp
# # 	$(CC) $(CFLAGS) -c $< -o $@

# # clean:
# # 	rm -rf $(OBJDIR) $(TARGET)

# # Targets
# all: $(OBJDIR) base

# $(OBJDIR):
# 	mkdir -p $(OBJDIR)

# $(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
# 	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# # Build the library
# base: $(OFILES)
# 	ar rv $(LIB) $^

# # Build the simulation
# sim: $(OFILES)
# 	$(MAKE) base
# 	$(CC) $(CFLAGS) main.cpp $(INCLUDES) -ldl -L./lib -lpba $(GLLDFLAGS) -o bin/lbmsim

# # Test compilation rules
# $(OBJDIR)/test_%.o: tests/%.cpp | $(OBJDIR)
# 	$(CC) $(TEST_CFLAGS) $(INCLUDES) -c $< -o $@

# $(OBJDIR)/test_%.o: tests/%.cpp | $(OBJDIR)
# 	$(CC) $(TEST_CFLAGS) $(INCLUDES) -c $< -o $@

# $(OBJDIR)/catch_amalgamated.o: $(CATCH_SRC) | $(OBJDIR)
# 	$(CC) $(TEST_CFLAGS) -c $< -o $@

# lib/libpba.a: base

# # Test target
# test_runner: $(TEST_OBJS) $(LIB)
# 	$(CC) $(TEST_CFLAGS) -o $@ $(TEST_OBJS) -L./lib -lpba $(GLLDFLAGS)

# # Convenience targets
# tests: test_runner

# test: test_runner
# 	./test_runner

# CLEANABLE_OBJS = $(filter-out $(OBJDIR)/catch_amalgamated.o,$(wildcard $(OBJDIR)/*.o))

# clean:
# 	rm -f $(CLEANABLE_OBJS) $(LIB) bin/pbalitesim test_runner

# cleanall:
# 	rm -rf $(OBJDIR) $(LIB) bin/pbalitesim test_runner

# .PHONY: all base sim tests test clean cleanall