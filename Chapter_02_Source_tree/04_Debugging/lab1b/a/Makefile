OBJECTS = a.o k.o l.o n.o r.o u.o
MODULES = a/a-mod2.o
MODULE = a-mod1.o

$(MODULE): $(OBJECTS) $(MODULES)
	$(LD) -r $^ -o $@

.PHONY: $(MODULES)
$(MODULES): 
	make -C $(dir $@)

.PHONY: clean
clean:
	make -C $(dir $(MODULES)) clean
	rm -f $(OBJECTS) $(MODULE)
