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

ifeq ($(DetectedOS), "Windows 64")

    CXX = x86_64-w64-mingw32-g++
    BuildDir = build/win64
    DistDir  = dist/win64
    EXEName  = Gerber2pdf_GUI.exe

    # Library Configuration

    OpenGLLibrary   = opengl32

    GLFW            = C:/TDM-GCC-64/lib/glfw-3.2.1
    GLFWInclude     = $(GLFW)/include
    GLFWLibInclude  = $(GLFW)/lib-ming-w64
    GLFWLibrary     = glfw3dll glfw3

    Boost           = C:/TDM-GCC-64/lib/boost_1_68_0
    BoostInclude    = $(Boost)
    BoostLibInclude = $(Boost)/stage/lib
    BoostLibrary    = boost_filesystem-mgw51-mt-x64-1_68 boost_system-mgw51-mt-x64-1_68

    DLLsToCopy      = $(GLFW)/lib-mingw-w64/glfw3.dll

else ifeq ($(DetectedOS), "Linux")

    BuildDir = build/linux
    DistDir  = dist/linux
    EXEName  = Gerber2pdf_GUI

    # Library Configuration

    OpenGLLibrary   = GL
    GLFWLibrary     = glfw
    BoostLibrary    = boost_filesystem boost_system

    # If CrossCompile is passed in via commandline,
    # use 32bit cross compiler provided by mingw-w64
    ifdef CrossCompile
        CXX = 'i686-w64-mingw32-g++'
        BuildDir = build/win32.cross
        DistDir  = dist/win32.cross
        EXEName  = Gerber2pdf_GUI.exe

        # Library Configuration

        OpenGLLibrary   = opengl32 gdi32
        GLFWLibrary     = glfw3
        BoostLibrary    = boost_filesystem-mt boost_system-mt

        _DLLsSrc        = /usr/i686-w64-mingw32/bin/
        _DLLsToCopy     = libstdc++-6.dll libgcc_s_sjlj-1.dll libwinpthread-1.dll \
                          libboost_system-mt.dll libboost_filesystem-mt.dll
        DLLsToCopy      = $(patsubst %, $(_DLLsSrc)/%, $(_DLLsToCopy))

    endif 
else
    $(error Sorry, your operating system is not currently supported. Please open an issue)
endif

# Path to Engine headers: Please modify as required
Engine = ../Source

FontsToCopy  = ./fonts/FiraSans-Regular.ttf

#-------------------------------------------------------------------------------
# General configuration

Options = -Wall -fexceptions -O2 -std=c++14

Includes   = -Iimgui -Iimgui/examples -Iimgui/misc/cpp \
             -I$(Engine)                               \
             -I$(Engine)/Tools/Deflate                 \
             -I$(Engine)/Tools/File                    \
             -I$(Engine)/Tools/Gerber                  \
             -I$(Engine)/Tools/PDF                     \
             -I$(Engine)/Tools/String                  \

ifneq ($(GLFWInclude),)
    Includes += -I$(GLFWInclude)
endif
ifneq ($(BoostInclude),)
    Includes += -I$(BoostInclude)
endif

Libraries  = $(patsubst %, -l%, $(GLFWLibrary))
Libraries += $(patsubst %, -l%, $(OpenGLLibrary))
Libraries += $(patsubst %, -l%, $(BoostLibrary))
Libraries += -lEngine -lm

LibInclude = -L$(Engine)/obj

ifneq ($(GLFWLibInclude),)
    LibInclude += -L$(GLFWLibInclude)
endif
ifneq ($(BoostLibInclude),)
    LibInclude += -L$(BoostLibInclude)
endif

DLLs = $(patsubst %, $(DistDir)/%, $(notdir DLLsToCopy))
Fonts = $(patsubst %, $(DistDir)/fonts/%, $(notdir FontsToCopy))

#-------------------------------------------------------------------------------

Objects = $(BuildDir)/obj/main.o                              \
          $(BuildDir)/obj/gui_main.o                          \
          $(BuildDir)/obj/logic.o                             \
          $(BuildDir)/obj/gerber_list_box.o                   \
          $(BuildDir)/obj/file_chooser.o                      \
          $(BuildDir)/obj/imgui/imgui.o                       \
          $(BuildDir)/obj/imgui/misc/cpp/imgui_stdlib.o       \
          $(BuildDir)/obj/imgui/examples/imgui_impl_glfw.o    \
          $(BuildDir)/obj/imgui/examples/imgui_impl_opengl2.o \
          $(BuildDir)/obj/imgui/imgui_draw.o                  \
          $(BuildDir)/obj/imgui/imgui_widgets.o

Headers = *.hpp \
          imgui/*.h imgui/examples/*.h imgui/misc/cpp/*.h

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

# DLLs

$(DLLs):
	mkdir -p $(@D)
	for dll in $(DLLsToCopy); do    \
		cp $$dll $(DistDir)/;       \
	done

$(Fonts): $(FontsToCopy)
	mkdir -p $(@D)
	for font in $(FontsToCopy); do  \
		cp $$font $(DistDir)/fonts; \
	done
#-------------------------------------------------------------------------------

