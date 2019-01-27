CC      = g++
Options = -std=c++11 -Wall -fexceptions -O2
#-------------------------------------------------------------------------------

Includes   = -ITools/Deflate
Includes  += -ITools/File
Includes  += -ITools/Gerber
Includes  += -ITools/Gerber/Gerber
Includes  += -ITools/PDF
Includes  += -ITools/PDF/afm
Includes  += -ITools/PDF/pdf

Libraries  = 
LibInclude = 
#-------------------------------------------------------------------------------

Version = -DMAJOR_VERSION=1 -DMINOR_VERSION=5

Objects  = obj/Tools/Deflate/JDeflate.o
Objects += obj/Tools/File/JFile.o
Objects += obj/Tools/Gerber/JGerber.o
Objects += obj/Tools/Gerber/Gerber/GerberAperture.o
Objects += obj/Tools/Gerber/Gerber/GerberLevel.o
Objects += obj/Tools/Gerber/Gerber/GerberMacro.o
Objects += obj/Tools/Gerber/Gerber/GerberRender.o
Objects += obj/Tools/PDF/JPDF.o
Objects += obj/Tools/PDF/afm/afmParser.o
Objects += obj/Tools/PDF/pdf/pdfAnnotation.o
Objects += obj/Tools/PDF/pdf/pdfArray.o
Objects += obj/Tools/PDF/pdf/pdfBool.o
Objects += obj/Tools/PDF/pdf/pdfCatalogue.o
Objects += obj/Tools/PDF/pdf/pdfContents.o
Objects += obj/Tools/PDF/pdf/pdfCustomFont.o
Objects += obj/Tools/PDF/pdf/pdfDest.o
Objects += obj/Tools/PDF/pdf/pdfDictionary.o
Objects += obj/Tools/PDF/pdf/pdfFont.o
Objects += obj/Tools/PDF/pdf/pdfFontDescriptor.o
Objects += obj/Tools/PDF/pdf/pdfFontFile.o
Objects += obj/Tools/PDF/pdf/pdfFontMetrics.o
Objects += obj/Tools/PDF/pdf/pdfForm.o
Objects += obj/Tools/PDF/pdf/pdfHyperlink.o
Objects += obj/Tools/PDF/pdf/pdfImage.o
Objects += obj/Tools/PDF/pdf/pdfIndirect.o
Objects += obj/Tools/PDF/pdf/pdfJPEG.o
Objects += obj/Tools/PDF/pdf/pdfName.o
Objects += obj/Tools/PDF/pdf/pdfNumber.o
Objects += obj/Tools/PDF/pdf/pdfObject.o
Objects += obj/Tools/PDF/pdf/pdfOpaque.o
Objects += obj/Tools/PDF/pdf/pdfOutlineItems.o
Objects += obj/Tools/PDF/pdf/pdfOutlines.o
Objects += obj/Tools/PDF/pdf/pdfPage.o
Objects += obj/Tools/PDF/pdf/pdfPages.o
Objects += obj/Tools/PDF/pdf/pdfPNG.o
Objects += obj/Tools/PDF/pdf/pdfRectangle.o
Objects += obj/Tools/PDF/pdf/pdfResources.o
Objects += obj/Tools/PDF/pdf/pdfStream.o
Objects += obj/Tools/PDF/pdf/pdfString.o
Objects += obj/Tools/PDF/pdf/pdfType1Font.o
Objects += obj/Tools/PDF/pdf/pdfType3Font.o
Objects += obj/Engine.o

ifneq ($(shell uname), Linux)
Resources = obj/Resources/resource.res
endif

Headers  = Tools/Deflate/*.h
Headers += Tools/File/*.h
Headers += Tools/Gerber/*.h
Headers += Tools/Gerber/Gerber/*.h
Headers += Tools/PDF/*.h
Headers += Tools/PDF/afm/*.h
Headers += Tools/PDF/pdf/*.h
Headers += *.h
#-------------------------------------------------------------------------------

.PHONY: clean all
.SECONDARY:

ifeq ($(shell uname), Linux)
all: bin/Gerber2pdf
else
all: bin/Gerber2pdf.exe
endif

clean:
	rm -rf bin obj
#-------------------------------------------------------------------------------

# Binaries

bin/Gerber2pdf: main.cpp main.h obj/libEngine.a
	mkdir -p bin
	$(CC) $(Options) $(Version) $(Includes) $< -s -Lobj -lEngine -o $@

bin/Gerber2pdf.exe: main.cpp main.h obj/libEngine.a $(Resources)
	mkdir -p bin
	$(CC) $(Options) $(Version) $(Includes) $< -s $(Resources) -Lobj -lEngine -o $@
#-------------------------------------------------------------------------------

# Libraries

obj/libEngine.a: $(Objects)
	mkdir -p $(@D)
	ar rcs $@ $(Objects)
#-------------------------------------------------------------------------------

# Objects

obj/%.o: %.cpp $(Headers)
	mkdir -p $(@D)
	$(CC) $(Options) $(Version) $(Defines) -c $(Includes) $< -o $@
#-------------------------------------------------------------------------------

# Resources

obj/%.res: %.rc
	mkdir -p $(@D)
	windres.exe -J rc -O coff -i $< -o $@
#-------------------------------------------------------------------------------

