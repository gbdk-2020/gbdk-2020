LCC	= lcc -Wa-l -Wl-m

BINS = banks_farptr.gb

all: obj $(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

obj:
	mkdir obj

obj/bank2code.o: src/bank2code.c
	$(LCC) -Wf-bo1 -c -o $@ $<

obj/banks_farptr.o: src/banks_farptr.c
	$(LCC) -c -o $@ $<

$(BINS): obj/banks_farptr.o obj/bank2code.o
	$(LCC) -Wl-yt1 -Wl-yo4 -o $@ $^

clean:
	rm -f obj/* $(BINS) *.ihx *.map *.noi


