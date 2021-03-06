// Based mainly on https://github.com/nanomsg/nng/blob/master/demo/async/client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/supplemental/util/platform.h>

void
fatal(const char *func, int rv)
{
  fprintf(stderr, "%s: %s\n", func, nng_strerror(rv));
  exit(1);
}

/*  The client runs just once, and then returns. */
int
client(const char *url)
{
  nng_socket sock;
  int        rv;
  nng_msg *  msg;
  nng_time   start;
  nng_time   end;

  if ((rv = nng_req0_open(&sock)) != 0) {
    fatal("nng_req0_open", rv);
  }

  if ((rv = nng_dial(sock, url, NULL, 0)) != 0) {
    fatal("nng_dial", rv);
  }

  start = nng_clock();

  if ((rv = nng_msg_alloc(&msg, 0)) != 0) {
    fatal("nng_msg_alloc", rv);
  }

  // Send a 32 bit uint round-trip to make sure the server is listening
  if ((rv = nng_msg_append_u32(msg, 0)) != 0) {
    fatal("nng_msg_append_u32", rv);
  }

  if ((rv = nng_sendmsg(sock, msg, 0)) != 0) {
    fatal("nng_send", rv);
  }

  if ((rv = nng_recvmsg(sock, &msg, 0)) != 0) {
    fatal("nng_recvmsg", rv);
  }

  end = nng_clock();
  nng_msg_free(msg);
  nng_close(sock);

  printf("Ping took %u milliseconds.\n", (uint32_t)(end - start));
  return (0);
}

int
main(int argc, char **argv)
{
  int rc;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <url>\n", argv[0]);
    fprintf(stderr, "Example: %s tcp://127.0.0.1:5555\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  rc = client(argv[1]);
  exit(rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
