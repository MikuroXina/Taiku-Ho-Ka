COMPILER  = clang++
CFLAGS    = -std=c++17 -g -O3 -Wall -Wextra -Winit-self -Wno-missing-field-initializers
ifeq "$(shell getconf LONG_BIT)" "64"
  LDFLAGS = -v
else
  LDFLAGS = -v
endif
LIBS      = /usr/local/lib/libftgl.a /usr/local/lib/libfreetype.a /usr/local/lib/libpng.a /usr/local/lib/libSDL2.dylib /System/Library/Frameworks/OpenGL.framework/Libraries/libGL.dylib /usr/local/Cellar/zlib/1.2.11/lib/libz.a  /usr/local/opt/bzip2/lib/libbz2.a /usr/local/lib/libSDL2_ttf.a /usr/local/lib/libSDL2_image.dylib /usr/local/lib/libSDL2_mixer.dylib
INCLUDE   = -I/usr/local/include
TARGET    = ./a.out
SRCDIR    = ./src
ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
