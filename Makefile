.PHONY: server clean debug

HOST_SRC := host/src
HOST_INC := host/include
SHARED_SRC := shared/src
SHARED_INC := shared/include

EXE := ggtag
OBJ := $(SHARED_SRC)/protocol.o $(SHARED_SRC)/GUI_Paint.o $(SHARED_SRC)/debug.o \
       $(SHARED_SRC)/font8.o $(SHARED_SRC)/font12.o \
	   $(SHARED_SRC)/font16.o $(SHARED_SRC)/font20.o \
	   $(SHARED_SRC)/font24.o $(SHARED_SRC)/font28.o \
	   $(SHARED_SRC)/font32.o $(SHARED_SRC)/font36.o $(SHARED_SRC)/fa.o \
	   $(SHARED_SRC)/qrcodegen.o $(HOST_SRC)/utils.o \
	   $(HOST_SRC)/ggtag.o $(HOST_SRC)/rfid.o

ifeq ($(CXX),emcc)
	EMFLAGS = -s EXPORTED_FUNCTIONS='["_render","_renderBuffer","_encode","_monoimage","_getLastError", "_debugEnable", "_malloc","_free"]' -s EXPORTED_RUNTIME_METHODS='["ccall","getValue","UTF8ToString"]' -s ALLOW_MEMORY_GROWTH=1
	EXE := docs/js/ggtag.js
endif
CXXFLAGS = -Wall -O2 -I$(SHARED_INC) -I$(HOST_INC)

all: $(EXE)

debug: CXXFLAGS += -g -DDEBUG -fsanitize=address -fsanitize=undefined
debug: LDFLAGS += -g -fsanitize=address -fsanitize=undefined
debug: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $^ -o $@ $(EMFLAGS) $(LDFLAGS)

$(SHARED_SRC)/%.o: $(SHARED_SRC)/%.c
	    $(CXX) $(CXXFLAGS) -c $< -o $@

$(HOST_SRC)/%.o: $(HOST_SRC)/%.cpp
	    $(CXX) $(CXXFLAGS) -c $< -o $@

server:
	python3 -m http.server --directory docs

clean:
	rm -f $(EXE) $(OBJ)
