SOURCE_DIR := source/
HEADER_DIR := header/
OBJECT_DIR := object/
OUTPUT := mplus

CXX := g++
CXXFLAGS := -Wall -O3 -std=c++11
CPPFLAGS := -I$(HEADER_DIR)

SOURCES := $(wildcard $(SOURCE_DIR)*.cxx)
OBJECTS := $(patsubst $(SOURCE_DIR)%.cxx,$(OBJECT_DIR)%.cxx.o,$(SOURCES))
DEPENDS := $(patsubst $(SOURCE_DIR)%.cxx,$(OBJECT_DIR)%.cxx.d,$(SOURCES))

all: build

build: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

-include $(DEPENDS)

$(OBJECT_DIR)%.cxx.o: $(SOURCE_DIR)%.cxx | $(OBJECT_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MMD -c -o $@ $<

$(OBJECT_DIR):
	mkdir $@

.PHONY: clean clean-all rebuild

clean:
	@rm -rf $(OBJECT_DIR)

clean-all: clean
	@rm $(OUTPUT)

rebuild: clean-all all
