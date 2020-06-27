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
	unsigned int a_t;
	unsigned int b_t;
	bool a_start;
	bool b_start;
	bool a_prev;
	bool b_prev;
	bool out;
};

struct coincidence butt1 = {100,0,0,0,0,0,0,0};

bool coincidence_detector(struct coincidence *c,bool a, bool b, unsigned int t) 
{
	printf("%u %u %u ", a, b, t);
    if ((2 == ((a<<1))+c->a_prev)) { // Rising edge
//        printf("Prev A %u Rise A\n",c->a_prev);
        c->a_t = t;
        c->a_start = true;
    }
    if ((2 == ((b<<1))+c->b_prev)) { // Rising edge
//        printf("Rise B\n");
        c->b_t =t;
        c->b_start = true;
    }
    if (a && b && c->a_start && c->b_start) {
        if (c->a_t > c->b_t) {
            if ((c->a_t - c->b_t) < c->delta) {
                c->out = true;
            }
            else {
                c->out = false;
				// Clear time measurment flags on time out.
				c->a_start = false;
				c->b_start = false;
            }
        }
        else if (c->b_t > c->a_t) {
            if ((c->b_t - c->a_t) < c->delta) {
                c->out = true;
            }
            else {
                c->out = false;
				// Clear time measurment flags on time out.
				c->a_start = false;
				c->b_start = false;
            }
        }
        else { // a_t == b_t
            c->out = true;
        }
    }
    if ((1 == ((a<<1))+c->a_prev)) { // Falling edge
        c->a_start = false;
    }
    if ((1 == ((b<<1))+c->b_prev)) { // Falling edge
        c->b_start = false;
    }
    if (!a || !b) c->out = false;
    c->a_prev = a;
    c->b_prev = b;
    return c->out;
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
    res =  coincidence_detector(&butt1,1,0,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,1,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,1,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,1,0,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,1,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,0,1,step(40)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,1,1,step(49)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,0,0,step(1)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,0,1,step(10)); printf("Out %d\n", res);
    res =  coincidence_detector(&butt1,1,1,step(10)); printf("Out %d\n", res);
#endif
#if 0
    res =  coincidence_detector(1,0,10); printf("Out %d\n", res);
    res =  coincidence_detector(1,0,20); printf("Out %d\n", res);
    res =  coincidence_detector(1,0,30); printf("Out %d\n", res);
#endif
    return 0;
}
