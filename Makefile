.PHONY: clean all Engine GUI

all: Engine GUI

Engine:
	make -C Engine all

GUI:
	make -C GUI    all

clean:
	make -C Engine clean
	make -C GUI    clean
#-------------------------------------------------------------------------------

