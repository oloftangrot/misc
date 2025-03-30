#define USE_CURSES
//#undef USE_CURSES
#undef USE_TERMINAL
#define STORE_RESULT
#include <math.h>
#include <stdio.h>
#ifdef USE_CURSES
#include <ncurses.h>
#endif
#include <time.h>
#include <unistd.h> /* usleep() */

//struct {double a1,a2,o1;} xorData[4] = {{0.,0.,0.},{1.,0.,1.},{0.,1.,1.},{1.,1.,0.}};
struct {double a1,a2,o1;} xorData[4] = {{0.,0.,1.},{1.,0.,0.},{0.,1.,0.},{1.,1.,1.}};

const int samples =4;
const int idata = 2;
const int hidden = 4;
const int odata = 1;


double sample[samples][idata];  	// Sample data for training
double ipdata[idata];			// Input data for network
double iout[idata];		       	// Output data from input units
double wih[idata][hidden];		// Weights between input and hidden units
double hin[idata][hidden];		// Input data to hidden units
double hsum[hidden];			// Sum of inputs to hidden units
double hout[hidden];			// Output data from hidden units
double who[hidden][odata];		// Weights between hidden an output units
double oin[odata][hidden];		// Input data to output units
double osum[odata];			// Sum of inputs to output units
double oout[odata];			// Output data from output units
double dout[samples][odata];            // Desired output
double obeta[odata];			// Error derivate for output units
double hbeta[hidden];			// Error derivate for hidden units
double hthr[hidden];			// Hidden threshold value
double othr[odata];				// Output threshold value
double oerror[samples]; 		// Network error

const double delta = 0.5 ;  // Setp size for weight adjustment
const double tolerance = 0.001; // Error tolerance for network

const char * inFileName = "example.dat";
const char * outFileName = "example.wht";

int noConvergence = 1;
int pass = 0;

void adjustWeights(void);
void calculateErrorForOutputLayer(int);
void calculateErrorForHiddenLayer(int);
void calculateNetworkError(int);
void calculateOutputs(int);
void checkForConvergence(void);
void displayTitles(void);
void readSampleData(void);
void setUpNetwork(void);
void storeCalculatedWeights(void);
void trainingMode( void );

int main ( void ) {
#ifdef USE_CURSES
	char buf[80];
	time_t T = time(NULL);
	struct tm tm;
	initscr();
	cbreak();
#endif
	setUpNetwork();
#ifdef USE_CURSES
	tm = *localtime(&T);
	sprintf(buf, "%02d:%02d:%02d",tm.tm_hour, tm.tm_min, tm.tm_sec);
	move(1, 40);
	printw("Start %s", buf );
	refresh();
#endif
	trainingMode();
#ifdef USE_CURSES
	T = time(NULL);
	tm = *localtime(&T);
	sprintf(buf, "%02d:%02d:%02d",tm.tm_hour, tm.tm_min, tm.tm_sec);
	move(1, 65);
	printw("Stop %s", buf );
	refresh();
	getch();

	clear();
	refresh();
#endif
	for ( int i = 0; i < samples; i++) {
		// Show desired out
#ifdef USE_CURSES
		move(5, 10);
		printw("%f", dout[i][0]);
#endif
		// Show network result
		calculateOutputs(i);
#ifdef USE_CURSES
		sleep(10);
#endif
	}
#ifdef USE_CURSES
	getch();
	endwin();
#endif
	return 0;
}

double rnd() {
	return ((double) rand()) / ((double) RAND_MAX);
}

void setUpNetwork ( void ) {
#ifdef USE_TERMINAL
	printf( "Setting up network\n");
#endif
	// Assign random values to weights between input and hidden layter
	for (int i = 0; i < idata; i++ ) {
		for ( int j = 0; j < hidden; j++ ) {
			wih[i][j] = rnd() * 2. - 1.;
		}
	}
	// Assign random values to threshold levels
	for ( int i = 0; i < hidden; i++ ) {
		hthr[i] = rnd() * 2. - 1.;
	}
	for ( int i = 0; i < odata; i++ ) {
		othr[i] = rnd() * 2. - 1.;
	}
	// Assign random values to weights between hidden and output layers
	for (int i = 0; i < hidden; i++ ) {
		for(int j = 0; j < odata; j++ ) {
			who[i][j] = rnd() * 2. - 1.;
		}
	}
}

void trainingMode( void ) {
#ifdef USE_TERMINAL
	printf("Starting training...\n");
#endif
	readSampleData();
	displayTitles();
	pass = 0;
	do {
		for ( int scan = 0; scan < samples; scan++ ) {
#ifdef USE_CURSES
			move(1,1);
			printw( "Pass = %d", pass );
			move(1,18);
			printw( "Sample = %d", scan );
			refresh();
#endif
			calculateOutputs(scan);
			calculateErrorForOutputLayer(scan);
			calculateErrorForHiddenLayer(scan);
			adjustWeights();
			calculateNetworkError(scan);
//			usleep(100000);
		}
		pass++;
		checkForConvergence();
	} while ( noConvergence );
#ifdef USE_CURSES
	move(22,45);
	printw( "Network has converged" );
	refresh();
#endif
#ifdef USE_TERMINAL
	printf( "Network has converged\n" );
#endif
	storeCalculatedWeights();
}

void readSampleData(void) {
#if 0
	FILE * inFile;
	inFile = fopen(inFileName, "r");
	if ( NULL == inFile ) {
		fprintf(stderr, "Error opening indata file!\n");
		exit(0);
	}
	// TODO add code for reading input data...
	fclose( inFile );
#endif
	// Initialize data from struct...
	for ( int i = 0; i < samples; i++ ) {
		sample[i][0] = xorData[i].a1;
		sample[i][1] = xorData[i].a2;
		dout[i][0] = xorData[i].o1;
	}
}

void calculateOutputs(int scan) {
	// Assign input data
	for ( int i = 0; i < idata; i++ ) {
		ipdata[i] = sample[scan][i];
	}
	// Calculate output of input data
	for ( int i = 0; i < idata; i++ ) {
		iout[i] = ipdata[i];
#ifdef USE_CURSES
		move(5+i,20);
		printw("%f", iout[i]);
#endif
	}
	// Calculate input to hidden units
	for ( int i = 0; i < idata; i++ ) {
		for ( int j = 0; j < hidden; j++ ) {
			hin[i][j] = iout[i] * wih[i][j];
		}
	}
	// Calculate output of hidden units
	for ( int j = 0; j < hidden; j++ ) {
		hsum[j] = 0;
		for ( int k = 0; k < idata; k++ ) {
			hsum[j] += hin[k][j];
		}
		hsum[j] += hthr[j];
		hout[j] = 1. / (1. + exp(-hsum[j]));
#ifdef USE_CURSES
		move(5+j, 31);
		printw("%f", hout[j]);
#endif
	}
	// Calculate inputs of output units
	for ( int j = 0; j < odata; j++ ) {
		for ( int k = 0; k < hidden; k++ ) {
			oin[j][k] = hout[k] * who[k][j];
		}
	}
	// Calculate outputs of output units
	for (int j = 0; j < odata; j++ ) {
		osum[j] = 0;
		for ( int k = 0; k < hidden; k++ ) {
			osum[j] += oin[j][k];
		}
		osum[j] += othr[j];
		oout[j] = 1. / ( 1. + exp(-osum[j]) );
#ifdef USE_CURSES
		move(5+j, 47);
		printw("%f", oout[j]);
#endif
	}
#ifdef USE_CURSES
	refresh();
#endif
}

void calculateErrorForOutputLayer(int scan) {
	for ( int i = 0; i < odata; i++ ) {
		obeta[i] = oout[i] - dout[scan][i];
	}
}

void calculateErrorForHiddenLayer(int scan) {
	for ( int j = 0; j < hidden; j++ ) {
		hbeta[j] = 0;
		for ( int k = 0; k < odata; k++ ) {
			hbeta[j] += who[j][k] * oout[k] * ( 1. - oout[k] ) * obeta[k];
		}
	}
}

void adjustWeights(void) {
	for ( int i = 0; i < idata; i++ ) {
		for ( int j = 0; j < hidden; j++ ) {
			wih[i][j] = wih[i][j] - ( delta * iout[i] * hout[j] * ( 1. - hout[j] ) * hbeta[j] );
		}
	}
	for ( int i = 0; i < hidden; i++ ) {
		hthr[i] = hthr[i] - ( delta * hout[i] * ( 1. - hout[i] ) * hbeta[i] );
	}
	for ( int i = 0; i < hidden; i++ ) {
		for ( int j = 0; j < odata; j++ ) {
			who[i][j] = who[i][j] - ( delta * hout[i] * oout[j] * ( 1. - oout[j] ) * obeta[j] );
		}
	}
	for ( int i = 0; i < odata; i++ ) {
		othr[i] = othr[i] - ( delta * oout[i] * ( 1. - oout[i] ) * obeta[i] );
	}
}

void calculateNetworkError( int scan ) {
	oerror[scan] = 0;
	for ( int i = 0; i < odata; i++ ) {
		oerror[scan] = oerror[scan] + ( oout[i] - dout[scan][i] ) * ( oout[i] - dout[scan][i] );
	}
	oerror[scan] = oerror[scan] / 2.;
//	move(10+scan,4);
//      printw("oerror %f", oerror[scan]);
}

void checkForConvergence( void ) {
	double neterror = 0;
	for ( int i = 0; i < samples; i++ ) {
		neterror += oerror[i];
	}
#ifdef USE_CURSES
	move(22, 1);
	printw( "Delta = %f Network error = %f", delta, neterror );
	refresh();
#endif
	if ( neterror < tolerance )
		noConvergence = 0;
	else
		noConvergence = 1;
	// IF RIGHT$(STR$(PASS), 1) = "00" THEN GOSUB storeCalculatedWeights
}

void storeCalculatedWeights( void ) {
#ifdef STORE_RESULT
	FILE * of;
	of = fopen( outFileName, "w" );
	if ( NULL == of ) {
		fprintf( stderr, "Error opening output file!\n" );
		exit(0);
	}
   	fprintf( of, "# DELTA\n" );
	fprintf( of, "%f\n", delta );
    	fprintf( of, "# WIH\n" );
    	for ( int i = 0; i < idata; i++ ) {
		for ( int j = 0; j < hidden; j++ ) {
			fprintf( of, "%f\n", wih[i][j] );
		}
	}
    	fprintf( of, "# HTHR\n" );
	for ( int i = 0; i < hidden; i++ ) {
		fprintf( of, "%f\n", hthr[i] );
	}
    	fprintf( of, "# WHO\n" );
    	for ( int j = 0; j < hidden; j++ ) {
		for ( int k = 0; k < odata; k++ ) {
			fprintf( of, "%f\n", who[j][k] );
		}
	}
    	fprintf( of, "# OTHR\n" );
	for ( int i = 0; i < odata; i++ ) {
		fprintf( of, "%f\n", othr[i] );
	}
	fclose( of );
#endif
#ifdef USE_CURSES
	move(23, 46);
	printw("Saving PASS %d to disk", pass );
	refresh();
#endif
}

void displayTitles( void ) {
#ifdef USE_CURSES
	move(3, 18);
	printw( "inputs    hidden nodes    output nodes" );
	move(4, 18);
	printw( "------    ------------    ------------" );
	refresh();
#endif
}
