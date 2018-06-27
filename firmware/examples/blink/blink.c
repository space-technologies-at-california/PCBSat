#include <msp430.h>

int main() {
    unsigned long i = 0;
    P1DIR = 0xff;

    while (1) {
        P1OUT = 0xff;
        for (i = 0; i < 10000; i++);
        P1OUT = 0;
        for (i =0; i < 10000; i++);
    }
}
