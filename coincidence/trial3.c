#include <stdbool.h>
#include <stdio.h>

/*
 * This coincidence detector will reset time measurements
 * only if time is out. This means that if an input toggles 
 * within the coincidence window the output will also
 * toggle.
 */
struct coincidence {
	unsigned int delta;
	unsigned int a_time;
	unsigned int b_time;
	bool a_start;
	bool b_start;
	bool a_prev;
	bool b_prev;
	bool out;
};

bool coincidence_detector(bool a, bool b, unsigned int t) 
{
    const unsigned int delta = 100;
    static unsigned int a_t = 0;
    static unsigned int b_t = 0;
    static bool a_prev = 0;
    static bool b_prev = 0;
    static int out = 0;
    static bool a_start = false;
    static bool b_start = false;

	printf("%u %u %u ", a, b, t);
    if ((2 == ((a<<1))+a_prev)) { // Rising edge
//        printf("Prev A %u Rise A\n",a_prev);
        a_t = t;
        a_start = true;
    }
    if ((2 == ((b<<1))+b_prev)) { // Rising edge
//        printf("Rise B\n");
        b_t =t;
        b_start = true;
    }
    if (a && b && a_start && b_start) {
        if (a_t > b_t) {
            if ((a_t - b_t) < delta) {
                out = true;
            }
            else {
                out = false;
				// Clear time measurment flags on time out.
				a_start = false;
				b_start = false;
            }
        }
        else if (b_t > a_t) {
            if ((b_t - a_t) < delta) {
                out = true;
            }
            else {
                out = false;
				// Clear time measurment flags on time out.
				a_start = false;
				b_start = false;
            }
        }
        else { // a_t == b_t
            out = true;
        }
    }
    if ((1 == ((a<<1))+a_prev)) { // Falling edge
        a_start = false;
    }
    if ((1 == ((b<<1))+b_prev)) { // Falling edge
        b_start = false;
    }
    if (!a || !b) out = false;
    a_prev = a;
    b_prev = b;
    return out;
}
/*
 * Increment time s units and save.
 */
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
    res =  coincidence_detector(1,0,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(1,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(1,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(1,0,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(1,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(0,1,step(40)); printf("Out %d\n", res);
    res =  coincidence_detector(1,1,step(49)); printf("Out %d\n", res);
    res =  coincidence_detector(0,0,step(1)); printf("Out %d\n", res);
    res =  coincidence_detector(0,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(1,1,step(10)); printf("Out %d\n", res);
#endif
#if 0
    res =  coincidence_detector(1,0,10); printf("Out %d\n", res);
    res =  coincidence_detector(1,0,20); printf("Out %d\n", res);
    res =  coincidence_detector(1,0,30); printf("Out %d\n", res);
#endif
    return 0;
}
