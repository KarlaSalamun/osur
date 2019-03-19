OBJECTS = k.o a.o r.o
MODULES = a/a-mod1.o
PROJECT = prog
    
$(PROJECT): $(OBJECTS) $(MODULES)
	$(CC) -o $@ $^ 

.PHONY: $(MODULES)
$(MODULES): 
	make -C $(dir $@)

.PHONY: clean
clean: 
	make -C $(dir $(MODULES)) clean
	rm -f $(OBJECTS) $(PROJECT)


