//
// Created by srkim on 23. 2. 19.
//

#include <pthread.h>
#include <stdint-gcc.h>
#include "../include/smtpInit.h"
#include "../include/smtpSvr.h"
#include "../include/logger.h"

int smtpInitThreads ( void ) {
    pthread_t   smtpTh  ;
    pthread_attr_t pAttr;
    unsigned long i;
    int nErr = 0;

    /* 1. Initialize Thread Attribute       */
    nErr = pthread_attr_init(&pAttr);
    nErr = pthread_attr_setstacksize(&pAttr, (20 * 1024 * 1024)); /* 20 MB*/

    //nErr = pthread_create ( &smtpTh , &pAttr , (void *)SMTP_WAIT_TH , NULL ) ;

    pthread_attr_destroy ( &pAttr ) ;

    LOG ( LOG_CRT , "< SIM > Thread Initialize ............................................... [ OK ]\n" ) ;

    return 0;
}


int smtpInitProcess( void )
{
    //smtpInitThreads();

    return 0;
}
