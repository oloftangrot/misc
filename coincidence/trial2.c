#include <stdbool.h>
#include <stdio.h>

/*
 * This coincidence detector will reset time measurements
 * only if time is out. This means that if an input toggles 
 * within the coincidence window the output will also
 * toggle.
 */
int foo(unsigned int a, unsigned int b, unsigned int t) 
{
    const unsigned int delta = 100;
    static unsigned int a_t = 0;
    static unsigned int b_t = 0;
    static unsigned int prev_a = 0;
    static unsigned int prev_b = 0;
    static int out = 0;
    static bool a_start = false;
    static bool b_start = false;

	printf("%u %u %u ", a, b, t);
    if ((2 == ((a<<1))+prev_a)) { // Rising edge
//        printf("Prev A %u Rise A\n",prev_a);
        a_t = t;
        a_start = true;
    }
    if ((2 == ((b<<1))+prev_b)) { // Rising edge
//        printf("Rise B\n");
        b_t =t;
        b_start = true;
    }
    if ((1 == a) && (1 == b) && a_start && b_start ) {
        if (a_t > b_t) {
            if ((a_t - b_t) < delta) {
                out = 1;
            }
            else {
                out = 0;
				// Clear time measurment flags on time out.
				a_start = false;
				b_start = false;
            }
        }
        else if (b_t > a_t) {
            if ((b_t - a_t) < delta) {
                out = 1;
            }
            else {
                out = 0;
				// Clear time measurment flags on time out.
				a_start = false;
				b_start = false;
            }
        }
        else { // a_t == b_t
            out = 1;
        }
    }
    if ((1 == ((a<<1))+prev_a)) { // Falling edge
        a_start = false;
    }
    if ((1 == ((b<<1))+prev_b)) { // Falling edge
        b_start = false;
    }
    if ((0 == a) || (0 == b)) out = 0;
    prev_a = a;
    prev_b = b;
    return out;
}

unsigned int step(unsigned int s) 
{
	static unsigned int t = 0;
	t+=s;
	return t;
}

int main(void)
{
    int res;
#if 1
    res = foo(1,0,step(10)); printf("Out %d\n", res);
    res = foo(1,1,step(10)); printf("Out %d\n", res);
    res = foo(1,1,step(10)); printf("Out %d\n", res);
    res = foo(1,0,step(10)); printf("Out %d\n", res);
    res = foo(1,1,step(10)); printf("Out %d\n", res);
    res = foo(0,1,step(40)); printf("Out %d\n", res);
    res = foo(1,1,step(49)); printf("Out %d\n", res);
    res = foo(0,0,step(1)); printf("Out %d\n", res);
    res = foo(0,1,step(10)); printf("Out %d\n", res);
    res = foo(1,1,step(10)); printf("Out %d\n", res);
#endif
#if 0
    res = foo(1,0,10); printf("Out %d\n", res);
    res = foo(1,0,20); printf("Out %d\n", res);
    res = foo(1,0,30); printf("Out %d\n", res);
#endif
    return 0;
}
