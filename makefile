TARGET := isingModel
BUILD_DIR := ./build
SRC_DIR := ./src

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
TXT := $(shell find ./ -name '*.txt' -or -name '*.dat')

PYTHON := -I/usr/include/python3.12 -lpython3.12
PYTHON_MAG := -I/usr/include/python3.10

CPP := g++
CPPFLAGS := -Wall -std=c++23 # $(AUTO_DEP)

magcompile:
	mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) $(PYTHON_MAG) $(SRCS) -lpython3.10 -lm -o $(BUILD_DIR)/$(TARGET)
	$(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $@
	$(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)
	rm -f $(TXT)

cleandata:
	rm -f $(TXT)

# -include $(DEPS)