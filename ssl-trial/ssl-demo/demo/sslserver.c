/*-
 * Opinion and code © 2009 Marco Peereboom.
 */
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <signal.h>
#include <sysexits.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SSL_CA_CRT	"../ca/ca.crt"
#define SSL_SERVER_CRT	"../server/server.crt"
#define SSL_SERVER_KEY	"../server/private/server.key"
#define	SSL_SERVER_PORT	4433

static int	work = 1;

static int	init_signals(void);
static void	onint(int);

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
	int			sock, s, r, val = -1;
	struct sockaddr_in	sin;

	if (init_signals() != EX_OK)
		err(EX_OSERR, "init signals failed");

	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	ctx = SSL_CTX_new(SSLv23_server_method());
	if (ctx == NULL)
		fatalx("ctx");

	if (!SSL_CTX_load_verify_locations(ctx, SSL_CA_CRT, NULL))
		fatalx("verify");
	SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(SSL_CA_CRT));

	if (!SSL_CTX_use_certificate_file(ctx, SSL_SERVER_CRT, SSL_FILETYPE_PEM))
		fatalx("cert");
	if (!SSL_CTX_use_PrivateKey_file(ctx, SSL_SERVER_KEY, SSL_FILETYPE_PEM))
		fatalx("key");
	if (!SSL_CTX_check_private_key(ctx))
		fatalx("cert/key");

	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
	SSL_CTX_set_verify_depth(ctx, 1);

	/* setup socket */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err(EX_OSERR, "socket");

	bzero(&sin, sizeof sin);
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SSL_SERVER_PORT);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val);

	if (bind(sock, (struct sockaddr *)&sin, sizeof sin) == -1)
		err(EX_OSERR, "bind");
	listen(sock, 0);

	for (; work != 0;) {
		if ((s = accept(sock, 0, 0)) == -1)
			err(EX_OSERR, "accept");

		sbio = BIO_new_socket(s, BIO_NOCLOSE);
		ssl = SSL_new(ctx);
		SSL_set_bio(ssl, sbio, sbio);

		if ((r = SSL_accept(ssl)) == -1)
			warn("SSL_accept");

		printf("received %d(%s) - handle it!\n", r,
		    ERR_reason_error_string(ERR_get_error()));

		BIO_free_all(sbio);
	}

	SSL_free(ssl);
	SSL_CTX_free(ctx);

	exit(EX_OK);
}

static int
init_signals(void)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = onint;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL)) {
		warn("sigaction(%d)", SIGINT);
		return (EX_OSERR);
	}

	return (EX_OK);
}

static void
onint(int s __attribute__((unused)) )
{
	work = 0;
}
