#include "cpptk.h"
#include <iostream>

using namespace Tk;
using namespace std;

void sayHello( void )
{
     cout << "Hello C++/Tk!" << endl;
}

void sayNay( void )
{
     cout << "Nay!" << endl;
}


int main( int argc, char *argv[] )
{
     try
     {
          init(argv[0]);
          
          button(".b1") -text("Say Hello") -command( sayHello );
          button(".b2") -text("Say Nay") -command( sayNay );
          pack(".b1") -padx(20) -pady(6);
          pack(".b2") -padx(20) -pady(6);
          
          runEventLoop();
     }
     catch (exception const &e)
     {
          cerr << "Error: " << e.what() << '\n';
     }
}
