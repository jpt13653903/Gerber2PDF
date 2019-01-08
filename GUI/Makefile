#-------------------------------------------------------------------------------

# OS Detection
ifeq ($(OS),Windows_NT)
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        DetectedOS = "Windows 64"
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            DetectedOS = "Windows 64"
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            DetectedOS = "Windows 32"
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        DetectedOS = "Linux"
    endif
endif

#-------------------------------------------------------------------------------

# Toolchain Configuration:
# Please edit the following section if you are using a different toolchain

Includes   = 
Libraries  = 
LibInclude = 

ifeq ($(DetectedOS), "Windows 64")
    CXX         = x86_64-w64-mingw32-g++
    BuildDir    = build/win64
    DistDir     = dist/win64
    EXEName     = Gerber2pdf_GUI.exe

    # Library Configuration

    Libraries  += -lopengl32

    GLFW        = C:/TDM-GCC-64/lib/glfw-3.2.1
    Includes   += -I$(GLFW)/include
    LibInclude += -L$(GLFW)/lib-mingw-w64
    Libraries  += -lglfw3dll -lglfw3

    Boost       = C:/TDM-GCC-64/lib/boost_1_68_0
    Includes   += -I$(Boost)
    LibInclude += -L$(Boost)/stage/lib
    Libraries  += -lboost_filesystem-mgw51-mt-x64-1_68 \
                  -lboost_system-mgw51-mt-x64-1_68

    DLLsToCopy  = $(GLFW)/lib-mingw-w64/glfw3.dll

else ifeq ($(DetectedOS), "Linux")
    BuildDir   = build/linux
    DistDir    = dist/linux
    EXEName    = Gerber2pdf_GUI

    # Library Configuration

	Libraries += -ldl # dlsym
    Libraries += -lGL
    Libraries += -lglfw
    Libraries += -lboost_filesystem -lboost_system

    # If CrossCompile is passed in via commandline,
    # use cross compiler provided by mingw-w64
    ifdef CrossCompile
        CXX         = 'i686-w64-mingw32-g++'
        BuildDir    = build/win32.cross
        DistDir     = dist/win32.cross
        EXEName     = Gerber2pdf_GUI.exe

        # Library Configuration

        Libraries  += -lopengl32 -lgdi32
        Libraries  += -lglfw3
        Libraries  += -lboost_filesystem-mt -lboost_system-mt

        _DLLsSrc    = /usr/i686-w64-mingw32/bin/
        _DLLsToCopy = libstdc++-6.dll libgcc_s_sjlj-1.dll libwinpthread-1.dll \
                      libboost_system-mt.dll libboost_filesystem-mt.dll
        DLLsToCopy  = $(patsubst %, $(_DLLsSrc)/%, $(_DLLsToCopy))
    endif
else
    $(error Sorry, your operating system is not currently supported. Please open an issue)
endif

# Path to Engine source: Please modify as required
Engine = ../Engine

FontsToCopy  = ./fonts/FiraSans-Regular.ttf
#-------------------------------------------------------------------------------

# General configuration

Options     = -Wall -fexceptions -O2 -std=c++14

Includes   += -Iimgui -Iimgui/examples -Iimgui/misc/cpp
Includes   += -I$(Engine)
Includes   += -I$(Engine)/Tools/Deflate
Includes   += -I$(Engine)/Tools/File
Includes   += -I$(Engine)/Tools/Gerber
Includes   += -I$(Engine)/Tools/PDF
Includes   += -I$(Engine)/Tools/String

Libraries  += -lEngine -lm
LibInclude += -L$(Engine)/obj

DLLs        = $(patsubst %, $(DistDir)/%, $(notdir DLLsToCopy))
Fonts       = $(patsubst %, $(DistDir)/fonts/%, $(notdir FontsToCopy))
#-------------------------------------------------------------------------------

Objects  = $(BuildDir)/obj/main.o
Objects += $(BuildDir)/obj/gui_main.o
Objects += $(BuildDir)/obj/logic.o
Objects += $(BuildDir)/obj/gerber_list_box.o
Objects += $(BuildDir)/obj/file_chooser.o
Objects += $(BuildDir)/obj/imgui/imgui.o
Objects += $(BuildDir)/obj/imgui/misc/cpp/imgui_stdlib.o
Objects += $(BuildDir)/obj/imgui/examples/imgui_impl_glfw.o
Objects += $(BuildDir)/obj/imgui/examples/imgui_impl_opengl2.o
Objects += $(BuildDir)/obj/imgui/imgui_draw.o
Objects += $(BuildDir)/obj/imgui/imgui_widgets.o

Headers  = *.hpp imgui/*.h imgui/examples/*.h imgui/misc/cpp/*.h
#-------------------------------------------------------------------------------

.PHONY: clean all
.SECONDARY:

all: $(DistDir)/$(EXEName) $(DLLs)

clean:
	rm -rf $(BuildDir)
	rm -rf $(DistDir)
#-------------------------------------------------------------------------------

# Binaries

$(DistDir)/$(EXEName): $(Objects) $(DLLs) $(Fonts)
	mkdir -p $(@D)
	$(CXX) $(Options) $(Includes) $(Objects) -s $(LibInclude) $(Libraries) -o $@
#-------------------------------------------------------------------------------

# Objects

$(BuildDir)/obj/%.o: %.cpp $(Headers)
	mkdir -p $(@D)
	$(CXX) $(Options) -c $(Includes) $< -o $@
#-------------------------------------------------------------------------------

# Resources

$(DLLs): $(DLLsToCopy)
	mkdir -p $(@D)
	if [ -n "$(DLLsToCopy)" ]; then		\
		cp -fv $(DLLsToCopy) $(@D)/;	\
	fi

$(Fonts): $(FontsToCopy)
	mkdir -p $(@D)
	cp -fv $(FontsToCopy) $(@D)/
#-------------------------------------------------------------------------------

