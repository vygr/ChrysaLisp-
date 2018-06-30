SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS :=  
CPPFLAGS := -O3 -std=c++14
CXXFLAGS += -MMD

chrysalisp: $(OBJ_FILES)
	c++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	c++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)/* chrysalisp

 -include $(OBJ_FILES:.o=.d)
