all: app tar
app: mksip32_dsp_cli
install: app
	cp ./vim3u3_fpga_cli /usr/bin
	ln -s /usr/bin/vim3u3_fpga_cli /usr/bin/fpga
mksip32_dsp_cli: fmap1.o cmd.o main.o tinyxml/tinystr.o tinyxml/tinyxml.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o
	$(CXX) fmap1.o main.o cmd.o tinyxml/tinystr.o tinyxml/tinyxml.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o -o vim3u3_fpga_cli

clean:
	rm -f *.o
	rm -f *~
	rm -f tinyxml/*.o
tar:
	rm -f sources.tar.gz
	tar -c -f sources.tar *.cpp *.h Makefile
	gzip -9 sources.tar
