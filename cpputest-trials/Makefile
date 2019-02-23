LFLAGS= -lCppUTest -lCppUTestExt -lgcov

all: trial1 trial2 trial3 trial4 trial5

trial1: trial1.cpp
	g++ -Wall -o $@ trial1.cpp -lCppUTest 

trial2: testMain.cpp trial2.cpp
	g++ -Wall -o $@ testMain.cpp trial2.cpp -lCppUTest 

trial3: testMain.o sum.o trial3.o
	g++ -Wall -o $@ $^ -lCppUTest -lgcov

trial4: testMain.o foo.o mock_sum.o trial4.o
	g++ -Wall -o $@ $^ -lCppUTest -lCppUTestExt -lgcov

trial5: testMain.o bar.o mock_systime.o trial5.o
	g++ -Wall -o $@ $^ -lCppUTest -lCppUTestExt -lgcov

mock_sum.o: mock_sum.cpp
	g++ -Wall -c $^ -o $@

mock_systime.o: mock_systime.cpp
	g++ -Wall -c $^ -o $@

sum.o: sum.c
	gcc -fprofile-arcs -ftest-coverage -fPIC -Wall -c $^ -o $@

foo.o: foo.c
	gcc -fprofile-arcs -ftest-coverage -fPIC -Wall -c $^ -o $@

bar.o: bar.c
	gcc -fprofile-arcs -ftest-coverage -fPIC -Wall -c $^ -o $@

testMain.o: testMain.cpp
	g++ -Wall -c $^ -o $@

trial3.o: trial3.cpp
	g++ -Wall -c $^ -o $@

trial4.o: trial4.cpp
	g++ -Wall -c $^ -o $@

trial5.o: trial5.cpp
	g++ -Wall -c $^ -o $@

cov:
	gcovr -r . --html --html-details -o coverage_report.html 

clean:
	rm	trial1 trial2 trial3 trial4 trial5 *.o *.gcno *.gcda *.html
