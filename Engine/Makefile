Options += -std=c++11 -Wall -fexceptions -O2

ifeq ($(OS), Windows_NT)
  CXX       = x86_64-w64-mingw32-g++
  obj       = obj_win
  Output    = bin/Gerber2pdf.exe
  Options  += -municode
  Resources = $(obj)/Resources/resource.res
else
  CXX       = g++
  obj       = obj_lin
  Output    = bin/Gerber2pdf
  Resources =
endif
#-------------------------------------------------------------------------------

Includes   = -ITools/Deflate
Includes  += -ITools/Gerber
Includes  += -ITools/Gerber/Gerber
Includes  += -ITools/PDF
Includes  += -ITools/PDF/afm
Includes  += -ITools/PDF/pdf
Includes  += -IToolbox/Cpp

Libraries  =
LibInclude =
#-------------------------------------------------------------------------------

Version = -DMAJOR_VERSION=1 -DMINOR_VERSION=7

Objects  = $(shell find Tools       -iname "*.cpp")
Objects += $(shell find Toolbox/Cpp -iname "*.cpp")
Objects += Engine.cpp
Objects := $(Objects:%.cpp=$(obj)/%.o)

Headers  = $(shell find . -iname "*.h")
#-------------------------------------------------------------------------------

.PHONY: clean all
.SECONDARY:

all: $(Output)

clean:
	rm -rf $(Output) $(obj)
#-------------------------------------------------------------------------------

# Binaries

$(Output): main.cpp main.h $(obj)/libEngine.a $(Resources)
	mkdir -p bin
	$(CXX) $(Options) $(Version) $(Includes) $< -s $(Resources) -L$(obj) -lEngine -o $@
#-------------------------------------------------------------------------------

# Libraries

$(obj)/libEngine.a: $(Objects)
	mkdir -p $(@D)
	ar -rcs $@ $(Objects)
#-------------------------------------------------------------------------------

# Objects

$(obj)/%.o: %.cpp $(Headers)
	mkdir -p $(@D)
	$(CXX) $(Options) $(Version) $(Defines) -c $(Includes) $< -o $@
#-------------------------------------------------------------------------------

# Resources

$(obj)/%.res: %.rc
	mkdir -p $(@D)
	windres.exe -J rc -O coff -i $< -o $@
#-------------------------------------------------------------------------------

