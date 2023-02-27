#!/bin/python
# -*- coding:utf-8 -*-

import smtplib
from email.mime.text import MIMEText

mail_to=[ "srkim@terracetech.co.kr", ]

def main():
    str_mail_to = ""
    for uid in mail_to:
        str_mail_to += "%s" % uid

    smtp = smtplib.SMTP('127.0.0.1', 2500)
    smtp.ehlo()
    msg = MIMEText('다우기술 메일보안파트 경력직 채용 과제 테스트 메일 입니다')
    msg['Subject'] = '테스트 메일입니다'
    msg['To'] = str_mail_to[0:-1]
    smtp.sendmail('freeis@terracetech.co.kr', 'srkim@terracetech.co.kr', msg.as_string())
    smtp.quit()


if __name__ == "__main__":
    n_send_mail = 1
    for idx in range(n_send_mail):
        print("send mail : [%d/%d]" % (idx, n_send_mail) )
        main()

