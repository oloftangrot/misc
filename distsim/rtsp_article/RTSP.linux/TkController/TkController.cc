#include "cpptk.h"
#include <iostream>
#include "RTSP.h"
#include "Parser.h"
#include <RTSP_Internals.h>

using namespace Tk;
using namespace std;

#define LB_ERR (0)

static RTSP* s_rtsp = NULL; // Create an RTSP object that is a controller (no name is necessary)
const unsigned int nameFieldWidth = 20;


class mfc_listbox {
public:
	int DeleteString( int i ) { 
		".f1.federation" << deleteitem( i );
		int c( ".f1.federation" << getsize()) ;
		return c; 
	};
  void AddString( const char* s ) { 
		".f1.federation" << insert( end, s );
	};
};

mfc_listbox m_ListBox;

// Local prototypes
//
void UpdateListBox( RTSP::MsgType msg_type ) ;
void LoadMDFFile(string file_name);
void ErrorMsgBox( const char * msg );
////
//
void OnPing( void )
{
	if (!s_rtsp)
		tk_messageBox() -messagetext( "No MDF file has been loaded" ) -icon( info );
	else {
		s_rtsp->CmdFederation( RTSP::Ping );
    UpdateListBox( RTSP::Ping ); 
	}
}

void OnInitialize( void )
{
	if (!s_rtsp)
		tk_messageBox() -messagetext( "No MDF file has been loaded" ) -icon( info );
	else {
		s_rtsp->CmdFederation( RTSP::Initialize, 2.0 );
    UpdateListBox( RTSP::Initialize );
	}
}

void OnStart( void )
{
	if (!s_rtsp)
		tk_messageBox() -messagetext( "No MDF file has been loaded" ) -icon( info );
	else {
		s_rtsp->CmdFederation( RTSP::Start );
    UpdateListBox( RTSP::Start );
	}
}

void OnHalt( void )
{
	if (!s_rtsp)
		tk_messageBox() -messagetext( "No MDF file has been loaded" ) -icon( info );
	else {
		s_rtsp->CmdFederation( RTSP::Halt );
    UpdateListBox( RTSP::Halt );
	}
}

void OnQuit( void )
{
	exit ( 0 );
}
/*
void OnOpen( void ) 
{
    bool do_open = true;
    char* def_ext = ".mdf";
    char* def_file_name = NULL;
    int flags = OFN_HIDEREADONLY;
    char* filter = "MDF files (*.mdf)|*.mdf||";

    CFileDialog file_dlg(do_open, def_ext, def_file_name, flags, filter);
    int result = file_dlg.DoModal();

    if (result == IDOK)
        LoadMDFFile(file_dlg.GetPathName());
*/
int main( int argc, char *argv[] )
{
	try
	{
		init(argv[0]);
		frame( ".f1" );
		frame( ".f1.s" );
		label( ".f1.s.federationState" ) -text( "Federation state:" );
		label( ".f1.s.state" ) -text( "Stopped" ) ;
    pack( ".f1.s" ) -Tk::fill( x );
		pack( ".f1.s.federationState" ) -padx( 2 ) -pady( 6 ) -side( Tk::left ) -anchor( "w" ) ;
		pack( ".f1.s.state" ) -side( Tk::left ) ;

		frame( ".f1.f" );
	
		label( ".f1.f.federateName" ) -text( "Federate name" ) ;
		label( ".f1.f.federateStatus" ) -text( "Federate status" ) ;
    pack( ".f1.f" ) -Tk::fill( x );
		pack( ".f1.f.federateName" ) -padx( 2 ) -pady( 1 ) -side( Tk::left ) ;
		pack( ".f1.f.federateStatus" ) -padx( 45 ) -side( Tk::left ) ;

    listbox( ".f1.federation" ) -width( 70 ) -font( "TkFixedFont" );
    pack( ".f1" ) -side( Tk::right ) ;
		pack( ".f1.federation" ) -padx( 2 ) -pady( 3 ) -anchor( "e" );


		frame( ".f2" )  -relief(raised) -borderwidth(1);
		button( ".f2.ping" ) -text("Ping") -relief("sunken")  -command( OnPing );
		button( ".f2.initialize" ) -text("Initialize") -command( OnInitialize );
		button( ".f2.start" ) -text("Start") -command( OnStart );
		button( ".f2.halt" ) -text("Halt") -command( OnHalt );
		button( ".f2.quit" ) -text("Quit") -command( OnQuit );
    pack(".f2") -Tk::fill( both ) -expand( 1 ) ;
		pack( ".f2.ping" ) -padx(20) -pady(6) -anchor( "w" );
		pack( ".f2.initialize" ) -padx(20) -pady(6) -anchor( "w" );
		pack( ".f2.start" ) -padx(20) -pady(6) -anchor( "w" );
		pack( ".f2.halt" ) -padx(20) -pady(6) -anchor( "w" );
		pack( ".f2.quit" ) -padx(20) -pady(6) -anchor( "w" );
     
		if ( argv[1] != NULL ) { // Expect the first argument to be a path to the MDF-file.
			LoadMDFFile( argv[1] );
		}    
//		m_ListBox.DeleteString(0) ;
    runEventLoop();
	}
	catch (exception const &e)
	{
		cerr << "Error: " << e.what() << '\n';
	}
}

void UpdateListBox(RTSP::MsgType msg_type) 
{
	char* cmd;
	bool* status_array;
	switch (msg_type)
	{
  case RTSP::Ping:
    cmd = (char*) "Ping";
    status_array = &_FederatePinged[0];
    break;
  case RTSP::Initialize:
    cmd = (char*) "Initialize";
    status_array = &_FederateInitialized[0];
    break;
  case RTSP::Start:
    cmd = (char*) "Start";
    status_array = &_FederateRunning[0];
    break;
  case RTSP::Halt:
    cmd = (char*) "Halt";
    status_array = &_FederateHalted[0];
    break;
  }

  // Delete all items currently in the list box
  while (m_ListBox.DeleteString(0) != LB_ERR)
    ;

  // Add the federate names and their initial state to the list
	int index = 0;
  for ( unsigned int i = 0; i < _NumFederateTypes; i++ )
    for ( uint j = 0; j < _FederateInfoList[i].dimension; j++ ) {
			std::string fed_name = " ";
      fed_name += _FederateInfoList[i].name;
			for ( unsigned int mm = 0; mm < ( nameFieldWidth - strlen( _FederateInfoList[i].name ) ); mm++ ) {
	      fed_name += " ";
			}
      if ( _FederateInfoList[i].dimension > 1 ) {
        char buf[20];
				sprintf(buf, "[%d]", j);
        fed_name += buf;
      }
      char* result;
      switch ( _FederateMsgResult[index] ) {
		    case Success:
		      result = (char*) "Success";
		      break;
		    case UserRoutineFailure:
		      result = (char*) "Failed";
		      break;
		    case InitFailed_Running:
		      result = (char*) "Can't initialize (running)";
		      break;
		    case StartFailed_Running:
		      result = (char*) "Can't start (running)";
		      break;
		    case StartFailed_NoInit:
		      result = (char*) "Can't start (not initialized)";
		      break;
		    case HaltFailed_NotRunning:
		      result = (char*) "Can't halt (not running)";
		      break;
      }
      fed_name += cmd;
      fed_name += ": ";

      if (status_array[index]) {
        if (msg_type == RTSP::Ping && _FederateMsgResult[index] == Success) {
          char buf[50];
			    sprintf(buf, "%.3f mS", 0.001*_ResponseTime[index]);
          fed_name += buf;
        }
        else
          fed_name += result;
      }
      else
        fed_name += "No response";

      m_ListBox.AddString( fed_name.c_str() );
			index++;
		}

    switch ( s_rtsp->FederationState() ) {
		  case RTSP::Stopped:
				".f1.s.state" << ( configure() -text( "Stopped" ) ) ;
		    break;
		  case RTSP::Initialized:
				".f1.s.state" << ( configure() -text( "Initialized" ) ) ;
		    break;
		  case RTSP::Running:
				".f1.s.state" << ( configure() -text( "Running" ) ) ;
		    break;
    }
//    UpdateData(false); // Is this some MFC? generic call
}

void LoadMDFFile(string file_name)
{
  ParseMDFFile( file_name, ErrorMsgBox );
  GenerateMDFData();

	delete s_rtsp;
	s_rtsp = new RTSP;

  // Delete all items currently in the list box
  while (m_ListBox.DeleteString(0) != LB_ERR)
    ;

  if (ErrorCount == 0) {    // Add the federate names and their initial state to the list
    for ( unsigned int i = 0; i < _NumFederateTypes; i++ ) {
      for (uint j=0; j<_FederateInfoList[i].dimension; j++) {
        std::string fed_name = " ";
        fed_name += _FederateInfoList[i].name;

        if (_FederateInfoList[i].dimension > 1) {
          char buf[20];
			    sprintf(buf, "[%d]", j);
          fed_name += buf;
        }
#if 0 // Fix fancy formatting later
        fed_name += "\tNot Connected";
#else
			for ( unsigned int mm = 0; mm < ( nameFieldWidth - strlen( _FederateInfoList[i].name ) ); mm++ ) {
	      fed_name += " ";
			}
      fed_name += "Not Connected";
#endif
        m_ListBox.AddString(fed_name.c_str());
	    }
		}
  }
}

void ErrorMsgBox( const char * msg )
{
	tk_messageBox() -messagetext( msg ) -icon( error );
}
