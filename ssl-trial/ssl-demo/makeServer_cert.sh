#!/bin/sh


mkdir -p server/private
chmod 700 server/private
openssl genrsa -out server/private/server.key 1024
openssl req -new -key server/private/server.key -out server/server.csr

openssl x509 -req -days 1460 -in server/server.csr -CA ca/ca.crt -CAkey ca/private/ca.key -CAcreateserial -out server/server.crt


