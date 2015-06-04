#!/bin/bash
# Run this for each email account.  The system must install the CA cert and the resulting p12 file in order to be happy.
 
# Borrowed from http://serverfault.com/questions/103263/can-i-create-my-own-s-mime-certificate-for-email-encryption
 
openssl genrsa -des3 -out smime.key 4096
openssl req -new -key smime.key -out smime.csr
openssl x509 -req -days 365 -in smime.csr -CA ca.crt -CAkey ca.key -set_serial 1 -out smime.crt -setalias "Self Signed SMIME" -addtrust emailProtection -addreject clientAuth -addreject serverAuth -trustout
openssl pkcs12 -export -in smime.crt -inkey smime.key -out smime.p12

