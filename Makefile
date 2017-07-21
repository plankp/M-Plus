SOURCE_DIR := source/
HEADER_DIR := header/
OBJECT_DIR := object/
OUTPUT := mplus

CXX := g++
CXXFLAGS := -Wall -O3
CPPFLAGS := -I$(HEADER_DIR)

SOURCES := $(wildcard $(SOURCE_DIR)*.cxx)
OBJECTS := $(patsubst $(SOURCE_DIR)%.cxx,$(OBJECT_DIR)%.cxx.o,$(SOURCES))

all: build

build: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

$(OBJECT_DIR)%.cxx.o: $(SOURCE_DIR)%.cxx
	@mkdir -p $(OBJECT_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

.PHONY: clean clean-all rebuild

clean:
	@rm -rf $(OBJECT_DIR)

clean-all: clean
	@rm $(OUTPUT)

rebuild: clean-all all
