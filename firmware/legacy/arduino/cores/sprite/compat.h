/**
 * compat.h -- MSP430 intrinsics got new names when it went to TI
 */

#define READ_SR __get_SR_register()
#define WRITE_SR __bis_SR_register
#define __bic_status_register_on_exit __bic_SR_register_on_exit
#define __bis_status_register __bis_SR_register

#define nop() __no_operation();
