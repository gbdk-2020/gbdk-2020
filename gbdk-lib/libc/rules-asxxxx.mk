$(LIB): pre $(OBJ)
	for file in $(OBJ) ; do \
		$(SDAR) -q $(LIB) $${file} ; \
	done
	$(SDAR) -d $(LIB) crt0.o

pre: set-model build-dir

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.s
	$(AS) -plosgff $@ $<
