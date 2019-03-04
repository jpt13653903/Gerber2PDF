ifeq ($(OS), Windows_NT)
  make = mingw32-make
else
  make = make
endif
#-------------------------------------------------------------------------------

.PHONY: clean all release Engine GUI

all: Engine GUI

clean:
	$(make) -C Engine clean
	$(make) -C GUI    clean

release: all
	scp ReadMe.md Engine/bin/* jptaylor@frs.sourceforge.net:/home/frs/project/gerber2pdf/
#-------------------------------------------------------------------------------

Engine:
	$(make) -C Engine all

GUI:
	$(make) -C GUI all
#-------------------------------------------------------------------------------

