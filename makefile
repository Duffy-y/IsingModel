TARGET := isingModel
BUILD_DIR := ./build
SRC_DIR := ./src

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TXT := $(shell find ./ -name '*.txt' -or -name '*.dat' -or -name '*.csv')

PYTHON := -I/usr/include/python3.12 -lpython3.12
PYTHON_MAG := -I/usr/include/python3.10

CPP := g++
CPPFLAGS := -Wall -std=c++23

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CPP) $(CPPFLAGS) $(PYTHON) $(OBJS) -o $@
	$(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CPP) $(CPPFLAGS) $(PYTHON) -c $< -o $@

.PHONY: compile magcompile

clean:
	rm -r $(BUILD_DIR)
	rm -f $(TXT)

cleandata:
	rm -f $(TXT)

compile:
	mkdir -p $(BUILD_DIR)
	$(CPP) $(CPPFLAGS) $(PYTHON) $(SRCS) -o $(BUILD_DIR)/$(TARGET)
	$(BUILD_DIR)/$(TARGET)

magcompile:
	mkdir -p $(BUILD_DIR)
	$(CPP) $(CPPFLAGS) $(PYTHON_MAG) $(SRCS) -lpython3.10 -lm -o $(BUILD_DIR)/$(TARGET)
	$(BUILD_DIR)/$(TARGET)