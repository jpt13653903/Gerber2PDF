.PHONY: clean all release Engine

all: Engine

clean:
	$(MAKE) -C Engine clean

release: all
	git push SourceForge master
	scp ReadMe.md Engine/bin/* jptaylor@frs.sourceforge.net:/home/frs/project/gerber2pdf/
#-------------------------------------------------------------------------------

Engine:
	$(MAKE) -C Engine all
#-------------------------------------------------------------------------------

