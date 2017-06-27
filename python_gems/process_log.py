import sys

filename = sys.argv[1]

infile = open( filename, "rb" ) 
outfile = open( "pentaho_input.csv", "wb" )

infile.readline()
infile.readline()
infile.readline()

try:
	byte = infile.read( 1 )
	while byte :
##		print( ord(byte) )
		if 10  ==  ord(byte)  : # Handle <LF>
			outfile.write( b',' )
		elif 58 == ord(byte) : # Handle ':' separating hh:mm:ss
			outfile.write( b',' )		
		elif 46 == ord(byte) : # Handle '.' separating mili seconds
			outfile.write( b',' )		
		elif 32 == ord(byte):  # Remove space
			pass
		elif 62 == ord(byte) : # Remove '>'
			pass
		else:	
			outfile.write( byte )
		if 13 == ord(byte)  : # Handle to <CR> -> <CR><LF>
			outfile.write( b'\n' )
			
		byte = infile.read( 1 )
finally:
	infile.close()

outfile.close()