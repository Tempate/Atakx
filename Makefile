CXX           = g++
FLAGS         = -std=c++17 -static
RELEASE_FLAGS = $(FLAGS) -O3 -DNDEBUG -flto -march=native
DEBUG_FLAGS   = $(FLAGS) -O0 -Wall -Wextra -g -Wall -Wextra -pedantic

LINKER  = g++

EXEC    = Funes
SRCDIR  = src
OBJDIR  = obj
BINDIR  = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(BINDIR)/$(EXEC): $(BINDIR) $(OBJDIR) $(OBJECTS)
	@$(LINKER) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CXX) $(FLAGS) -c $< -o $@

release:
	$(MAKE) FLAGS="$(RELEASE_FLAGS)"

debug:
	$(MAKE) FLAGS="$(DEBUG_FLAGS)" EXEC="$(EXEC)-debug"

bin:
	mkdir -p $(BINDIR)

obj:
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJECTS)

.PHONY: clean
