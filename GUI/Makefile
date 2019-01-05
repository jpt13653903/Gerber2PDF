ifeq ($(shell uname), Linux)
$(error Use on Windows only.  On Linux, use the meson build chain.)
endif
#-------------------------------------------------------------------------------

CC = x86_64-w64-mingw32-g++

Options = -Wall -fexceptions -O2 -std=c++14
#-------------------------------------------------------------------------------

# User-specific paths

GLFW   = C:/TDM-GCC-64/lib/glfw-3.2.1
Boost  = C:/TDM-GCC-64/lib/boost_1_68_0
Engine = ../Source
#-------------------------------------------------------------------------------

Includes   = -Iimgui -Iimgui/examples -Iimgui/misc/cpp \
             -I$(Engine)                               \
             -I$(Engine)/Tools/Deflate                 \
             -I$(Engine)/Tools/File                    \
             -I$(Engine)/Tools/Gerber                  \
             -I$(Engine)/Tools/PDF                     \
             -I$(Engine)/Tools/String                  \
             -I$(GLFW)/include                         \
             -I$(Boost)

Libraries  = -lopengl32 -lglfw3dll -lglfw3
Libraries += -lboost_filesystem-mgw51-mt-x64-1_68
Libraries += -lboost_system-mgw51-mt-x64-1_68 
Libraries += -lEngine

LibInclude = -L$(GLFW)/lib-mingw-w64 \
             -L$(Boost)/stage/lib    \
	     -L$(Engine)/obj
#-------------------------------------------------------------------------------

Objects = build.win/obj/main.o                              \
          build.win/obj/gui_main.o                          \
          build.win/obj/logic.o                             \
          build.win/obj/gerber_list_box.o                   \
          build.win/obj/imgui/imgui.o                       \
          build.win/obj/imgui/misc/cpp/imgui_stdlib.o       \
          build.win/obj/imgui/examples/imgui_impl_glfw.o    \
          build.win/obj/imgui/examples/imgui_impl_opengl2.o \
          build.win/obj/imgui/imgui_draw.o                  \
          build.win/obj/imgui/imgui_widgets.o

Headers = *.hpp \
          imgui/*.h imgui/examples/*.h imgui/misc/cpp/*.h

DLLs = build.win/glfw3.dll
#-------------------------------------------------------------------------------

.PHONY: clean all FORCE
.SECONDARY:

all: build.win/Gerber2pdf_GUI.exe $(DLLs)

clean:
	rm -rf build
	rm -rf build.win

FORCE:
#-------------------------------------------------------------------------------

# Binaries

build.win/Gerber2pdf_GUI.exe: $(Objects) $(DLLs) $(Engine)/obj/libEngine.a
	mkdir -p $(@D)
	$(CC) $(Options) $(Includes) $(Objects) -s $(LibInclude) $(Libraries) -o $@
#-------------------------------------------------------------------------------

# Objects

build.win/obj/%.o: %.cpp $(Headers)
	mkdir -p $(@D)
	$(CC) $(Options) -c $(Includes) $< -o $@

$(Engine)/obj/libEngine.a: FORCE
	make -C $(Engine)
#-------------------------------------------------------------------------------

# DLLs

build.win/glfw3.dll:
	mkdir -p $(@D)
	cp -f $(GLFW)/lib-mingw-w64/glfw3.dll $@
#-------------------------------------------------------------------------------

