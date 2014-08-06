all:
	cd src; make

.PHONY: clean
clean:
	cd src; make clean

.PHONY: install
install:
	cd src; make; make install

.PHONY: run
installrun:
	cd src; make; ./mergesac
