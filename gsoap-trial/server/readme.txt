1) Skapa interface fil från en mall på ett känt ställe...

wsdl2h -o calc.h http://www.genivia.com/calc.wsdl

2) Skapa skelett filerna...

soapcpp2 -j -SL calc.h

(-I/path/to/gsoap/import ) har utelämnats
