OBJECTS = a.o s.o a2.o m.o 
MODULES = a/a-mod3.o
MODULE = a-mod2.o

$(MODULE): $(OBJECTS) $(MODULES)
	$(LD) -r $^ -o $@ 

.PHONY: $(MODULES)
$(MODULES):
	make -C $(dir $@)

.PHONY: clean
clean:
	make -C $(dir $(MODULES)) clean
	rm -f $(OBJECTS) $(MODULES)
