all: gsoap-trial-server

# LIBS=-lgsoap++

# Rule to make the C++ server
gsoap-trial-server: main.cpp soapcalcService.cpp
	g++ main.cpp soapcalcService.cpp soapC.cpp -o gsoap-trial-server -lgsoap++


# Rule to auto generate files form the calc prototype file (exclueded -I/path/to/gsoap/import)
soapcalcService.cpp: calc.h
	soapcpp2 -j -SL  calc.h

# Rule to create the prototype file from an existing Web-service location.
calc.h: typemap.dat
	wsdl2h -t./typemap.dat -o calc.h http://www.genivia.com/calc.wsdl

# Rule to create a replacement for ns to c as annotation for the created prototypes
typemap.dat:
	echo 'c ="urn:calc"' > typemap.dat

# Rule to remove the build target and other files created by the this Makefile
clean:
	rm gsoap-trial-server calc.h soapcalcService.cpp soapC.cpp
	rm calc.nsmap soapH.h soapStub.h soapcalcService.h
	rm typemap.dat
	rm *.xml
