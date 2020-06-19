### Edit this variable to add new compiler flags (sanitizers, debugging, etc...)
EXTRA_FLAGS := -Wall -Wextra
CXX			:= clang++

###################################
### DO NOT EDIT BELOW THIS LINE ###
###################################

BUILDDIR	:= build
OUTDIR		:= $(BUILDDIR)/bin
TEMPDIR		:= $(BUILDDIR)/tmp
MKDIR_P		:= mkdir -p

CXXFLAGS:= $(CXXFLAGS) -std=c++11 $(EXTRA_FLAGS)
INCLUDE	:= $(patsubst %,-I%,.)

FILES	:= AlternationExpression.cpp AnyCharacterExpression.cpp EmptyExpression.cpp LazyRepeatRangeExpression.cpp LiteralExpression.cpp RegularExpression.cpp RepeatNExpression.cpp RepeatRangeExpression.cpp SearchExpression.cpp parse.cpp
OBJECTS	:= $(patsubst %.cpp,$(TEMPDIR)/%.o,$(FILES))
LIBRARY	:= $(TEMPDIR)/libregex.a
TARGET	:= $(OUTDIR)/fuzz

VPATH := $(TEMPDIR)

.PHONY: all clean makedirs lib fuzz

all: makedirs | fuzz

clean:
	rm -rf $(OUTDIR) $(TEMPDIR)

makedirs:
	$(MKDIR_P) $(OUTDIR) $(TEMPDIR)

$(TEMPDIR)/%.o: makedirs | %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $| -o $@

$(LIBRARY): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(TARGET): makedirs | fuzz.cpp $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $(LDFLAGS) $| $(LIBS)

lib: $(LIBRARY)

fuzz: $(LIBRARY) $(TARGET)