#!/bin/sh
openssl s_client -CAfile ca/ca.crt -cert client/client.crt -key client/private/client.key
