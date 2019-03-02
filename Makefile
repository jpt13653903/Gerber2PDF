ifeq ($(OS), Windows_NT)
  make = mingw32-make
else
  make = make
endif
#-------------------------------------------------------------------------------

.PHONY: clean all release Engine

all: Engine

clean:
	$(make) -C Engine clean

release: all
	scp ReadMe.md Engine/bin/* jptaylor@frs.sourceforge.net:/home/frs/project/gerber2pdf/
#-------------------------------------------------------------------------------

Engine:
	$(make) -C Engine all
#-------------------------------------------------------------------------------

