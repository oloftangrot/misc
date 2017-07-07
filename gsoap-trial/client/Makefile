all: gsoap-trial-client

LIBS=-lgsoap++

# Rule to make the C++ client
gsoap-trial-client: soapcalcProxy.cpp main.cpp
	g++ main.cpp soapcalcProxy.cpp soapC.cpp -o gsoap-trial-client -lgsoap++

# Rule to auto generate files form the calc prototype file (exclueded -I/path/to/gsoap/import)
soapcalcProxy.cpp: calc.h
	soapcpp2 -j -CL  calc.h

# Rule to create the prototype file from an existing Web-service location.
calc.h: typemap.dat
	wsdl2h -t./typemap.dat -o calc.h http://www.genivia.com/calc.wsdl

# Rule to create a replacement for ns to c as annotation for the created prototypes
typemap.dat:
	echo 'c ="urn:calc"' > typemap.dat

# Rule to remove the build target and other files created by the this Makefile
clean:
	rm gsoap-trial-client calc.h soapcalcProxy.cpp soapC.cpp
	rm calc.nsmap soapH.h soapStub.h soapcalcProxy.h
	rm typemap.dat
	rm *.xml
