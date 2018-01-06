#!/bin/sh
openssl s_server -CAfile ca/ca.crt -cert server/server.crt -key server/private/server.key -Verify 1

