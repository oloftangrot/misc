/*-
 * Opinion and code © 2009 Marco Peereboom.
 */
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sysexits.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#define SSL_CA_CRT	"../ca/ca.crt"
#define	SSL_CLIENT_CRT	"../client/client.crt"
#define SSL_CLIENT_KEY	"../client/private/client.key"
#define	SSL_SERVER_PORT	4433

void
fatalx(char *s)
{
	ERR_print_errors_fp(stderr);
	errx(EX_DATAERR, "%.30s", s);
}

int
main(int argc, char *argv[])
{
	SSL_CTX			*ctx;
	BIO			*sbio;
	SSL			*ssl;
	struct sockaddr_in	addr;
	struct hostent		*hp;
	int			sock;


	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	ctx = SSL_CTX_new(SSLv23_client_method());
	if (ctx == NULL)
		fatalx("ctx");

	if (!SSL_CTX_load_verify_locations(ctx, SSL_CA_CRT, NULL))
		fatalx("verify");

	if (!SSL_CTX_use_certificate_file(ctx, SSL_CLIENT_CRT, SSL_FILETYPE_PEM))
		fatalx("cert");
	if (!SSL_CTX_use_PrivateKey_file(ctx, SSL_CLIENT_KEY, SSL_FILETYPE_PEM))
		fatalx("key");
	if (!SSL_CTX_check_private_key(ctx))
		fatalx("cert/key");

	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify_depth(ctx, 1);

	/* setup connection */
	if ((hp = gethostbyname("localhost")) == NULL)
		err(EX_OSERR, "gethostbyname");

	bzero(&addr, sizeof addr);
	addr.sin_addr = *(struct in_addr *)hp->h_addr_list[0];
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SSL_SERVER_PORT);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err(EX_OSERR, "socket");
	if (connect(sock, (struct sockaddr *)&addr, sizeof addr) == -1)
		err(EX_OSERR, "connect");

	/* go do ssl magic */
	ssl = SSL_new(ctx);
	sbio = BIO_new_socket(sock, BIO_NOCLOSE);
	SSL_set_bio(ssl, sbio, sbio);

	if (SSL_connect(ssl) <= 0)
		fatalx("SSL_connect");

	if (SSL_get_verify_result(ssl) != X509_V_OK)
		fatalx("cert");

	printf("connected to server!\n");

//	BIO_free_all(sbio);
	SSL_free(ssl);
	SSL_CTX_free(ctx);

	exit(EX_OK);
}
