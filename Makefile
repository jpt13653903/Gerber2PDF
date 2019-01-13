.PHONY: clean all Engine

all: Engine

Engine:
	make -C Engine all

clean:
	make -C Engine clean
#-------------------------------------------------------------------------------

