OBJECTS = a.o l.o 
MODULE = a-mod3.o

$(MODULE): $(OBJECTS) 
	$(LD) -r $^ -o $@ 

.PHONY: clean
clean: 
	rm -f $(OBJECTS) $(MODULE)
