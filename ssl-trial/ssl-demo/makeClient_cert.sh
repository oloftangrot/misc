#!/bin/sh


mkdir -p client/private
chmod 700 client/private
openssl genrsa -out client/private/client.key 1024
openssl req -new -key client/private/client.key -out client/client.csr
echo "Sign cert with CA key"
openssl x509 -req -days 1460 -in client/client.csr -CA ca/ca.crt -CAkey ca/private/ca.key -CAcreateserial -out client/client.crt

