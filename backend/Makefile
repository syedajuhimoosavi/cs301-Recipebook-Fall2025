CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.
LDFLAGS = -lsqlite3 -lpthread

TARGET = recipe_server
SOURCES = main.cpp database.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) recipes.db

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
