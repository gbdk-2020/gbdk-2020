$(LIB): pre $(OBJ)
	for file in $(filter-out %/crt0.o,$(OBJ)) ; do \
		$(SDAR) -ru $(LIB) $${file} ; \
	done

pre: set-model build-dir

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.s
	$(AS) -plosgff $@ $<
