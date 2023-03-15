#include "main.h"

/*  TODO 과제 1 공통 사항
 *  session 관리는 해쉬테이블로 관리된다.
 *  session 관리 시 충돌을 방지하여야 한다.
 *  hash_id는 해당 파일에서 제공하는 hash_func을 사용한다.
 */

int hash_func(const char *string, size_t len) {
    int i;
    int hash;

    hash = 0;
    for (i = 0; i < len; ++i) {
        hash = 65599 * hash + string[i];
    }

    return hash ^ (hash >> 16);
}

void delSmtpSession(char *session_id) {
    /*  TODO 과제 1-1
     *   smtp 세션을 종료하려고 한다.
     *   그동안 사용되었던 session은 현재 해시테이블을 이용하여 관리되고 있었는데
     *   해당 테이블에 일치하는 session정보를 전달받은 session id를 이용하여 제거하는 로직을 개발하시오.
     */
    return;
}

smtp_session_t *getSmtpSession(char *session_id) {
    /*  TODO 과제 1-2
     *   session_id에 일치하는 session 정보를 return 해주어야 한다.
     *   해시테이블에 존재하는 session중 session_id에 일치하는 session을 반환해주시오
     */

    return null;
}

smtp_session_t *addSmtpSession(smtp_session_t *session) {
    /*  TODO 과제 1-3
     *   smtp 세션을 추가하려고 한다.
     *   전달받은 session정보를 활용하여 현재 관리하고 있는 해시테이블에 추가하는 로직을 개발하시오.
     */
    return null;
};