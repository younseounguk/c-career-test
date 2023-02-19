#include <stdio.h>
#include <stdint-gcc.h>
#include "include/logger.h"
#include "include/smtpSvr.h"
#include "include/smtpInit.h"
#include "include/smtpUtils.h"

int main() {

    /* 1. Initialize Processing */
    if ( smtpInitProcess () < 0 ) return (-1) ;

    smtpWait();
    return 0;
}
