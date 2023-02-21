//
// Created by srkim on 23. 2. 21.
//

#include "main.h"

int saveMailAsFile(SmtpSession_t * session) {
    FILE *  fp;

    if( ! ( fp = fopen ( EML_FILE_NAME , "wb" ))) {     /* do open new file, if not do error */

        LOG( LOG_MAJ , "create %s failed(%s)\n", EML_FILE_NAME , strerror(errno) ) ;
        return (-1) ;
    }
    fprintf(fp, "%s", session->smtpData);
    fclose(fp);
    return 0;
}
