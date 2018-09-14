#include <stdbool.h>

#include "lsm.h"

/* main.c */
bool radio_precond();
void sleep(uint16_t ms, unsigned short mode);
void deep_sleep(uint16_t ms);
int main(void);
/* radio.c */
void run_radio(void);

struct vec3_s global_omega;
extern char tx_msg[7];
