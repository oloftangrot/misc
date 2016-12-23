#include "calc.nsmap"      // XML namespace mapping table (only needed once at the global level)
#include "soapcalcProxy.h" // the proxy class, also #includes "soapH.h" and "soapStub.h"

int main()
{
  calcProxy calc;
	calc.soap_endpoint = "http://localhost:8080/";
  double sum;
  if (calc.add( 1.23, 4.56, sum) == SOAP_OK)
//  if (calc.add(1.23, 4.56, sum) == SOAP_OK)
    std::cout << "Sum = " << sum << std::endl;
  else
    calc.soap_stream_fault(std::cerr);
  calc.destroy(); // same as: soap_destroy(calc.soap); soap_end(calc.soap);
}
