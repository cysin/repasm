all: repasm.so


repasm.so:
	$(MAKE) -CStormLib && $(MAKE) -CBWlib && $(MAKE) -CRXFile && $(MAKE) -CRXMapBase && $(MAKE) -CBWTools && $(MAKE) -CRXMapTools && $(MAKE) -Crepasm
	
clean:
	rm BWlib/*.o ; rm BWTools/*.o ; rm libs/* ; rm RXFile/*.o ; rm RXMapBase/*.o ; rm RXMapTools/*.o ; rm repasm/*.o ; find StormLib -name *.o -delete
