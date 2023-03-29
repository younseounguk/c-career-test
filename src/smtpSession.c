#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 Socket정보를 보관하기 위해 사용한다.
 *  다량의 session 정보를 보관하는 보관소가 존재하여야 한다.
 *  session 관리 시 충돌을 방지하여야 한다.
 */

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료하려고 한다.
     *   그동안 사용되었던 session을 session_id를 이용하여 session 보관소에서 제거하는 로직을 개발하시오.
     */
    return;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-2
     *   smtp 세션을 추가하려고 한다.
     *   전달받은 session정보를 활용하여 현재 관리하고 있는 session 보관소에 추가하는 로직을 개발하시오.
     */
    return NULL;
};