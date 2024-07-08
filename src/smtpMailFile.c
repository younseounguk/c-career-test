#include "main.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
int saveMailAsFile(smtp_session_t * session) {
	FILE *  fp;
	if( ! ( fp = fopen ( EML_FILE_NAME , "wb" ))) {     /* do open new file, if not do error */

		LOG( LOG_MAJ , "create %s failed(%s)\n", EML_FILE_NAME , strerror(errno) ) ;
		return (-1) ;
	}
	fprintf(fp, "%s", session->smtp_data);
	fclose(fp);
	return 0;
}
