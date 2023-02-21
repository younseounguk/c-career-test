#include "main.h"

int main() {

    /* 1. Initialize Processing */
    if ( smtpInitProcess () < 0 ) return (-1) ;

    smtpWait();
    return 0;
}
