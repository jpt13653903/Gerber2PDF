.PHONY: clean all release Engine

all: Engine

clean:
	make -C Engine clean

release: all
	scp ReadMe.md Engine/bin/* jptaylor@frs.sourceforge.net:/home/frs/project/gerber2pdf/
#-------------------------------------------------------------------------------

Engine:
	make -C Engine all
#-------------------------------------------------------------------------------

