COMPILER  = clang++
CFLAGS    = -std=c++14 -g -MMD -MP -Wall -Wextra -Winit-self -Wno-missing-field-initializers -v
ifeq "$(shell getconf LONG_BIT)" "64"
  LDFLAGS = -v
else
  LDFLAGS = -v
endif
LIBS      = /usr/local/Cellar/ftgl/2.1.3-rc5/lib/libftgl.a /usr/local/opt/freetype/lib/libfreetype.a /usr/local/opt/libpng/lib/libpng.a /usr/local/opt/sdl2/lib/libSDL2.dylib /System/Library/Frameworks/OpenGL.framework/Libraries/libGL.dylib /usr/local/Cellar/zlib/1.2.11/lib/libz.a  /usr/local/opt/bzip2/lib/libbz2.a /usr/local/opt/sdl2_ttf/lib/libSDL2_ttf.a /usr/local/opt/sdl2_image/lib/libSDL2_image.dylib /usr/local/opt/sdl2_mixer/lib/libSDL2_mixer.dylib
INCLUDE   = -I./include -framework SDL2 -framework Cocoa -I/Library/Frameworks/SDL2.framework/Headers/ -I/Library/Frameworks/SDL2_ttf.framework/Headers/ -I/usr/local/include/freetype2/ -I/usr/local/opt/bzip2/include  -I/usr/local/opt/sdl2_image/include/SDL2/ -I/usr/local/opt/sdl2_mixer/include/SDL2/
TARGET    = ./bin.out
SRCDIR    = ./
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
