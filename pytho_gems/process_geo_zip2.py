import re
test_f = open( 'SE.txt' )
result_f = open( 'result.txt', 'w' )
for line in test_f:
#    new_str = re.sub('[^a-zA-Z0-9\n\.]'," ", line)
    new_str = line.replace( " ", "" )
    result_f.write( new_str )

# also, this too, please:
test_f.close()
result_f.close()