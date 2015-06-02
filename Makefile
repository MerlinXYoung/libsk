#
# Main makefile of libsk.
#

LIBSK_HOME = .

include ./inc.mk

.PHONY: clean

all:
	@date;
	@cd $(LIBSK_SRC); make -j8 all;
	@cd $(LIBSK_PERF); make -j8 all;
	@cd $(LIBSK_TEST); make -j8 all;
	@date;

clean:
	@date;
	@cd $(LIBSK_SRC); make clean;
	@cd $(LIBSK_PERF); make clean;
	@cd $(LIBSK_TEST); make clean;
	@date;
