/* "Multiplxed_Request" is the third of our Download Manager. 
 * This program allows to download file(s) using multiplexed 
 * functionality of HTTP/2 technology. Generally it is
 * faster than "Pipelined_Request" and "Regular_Request".
 *
 * The original implementation of this program has been comp-
 * leted by Daniel Stenberg. You can find it in this link:
 * http://curl.haxx.se/libcurl/c/http2-download.html. I have
 * changed some details in order to make it more project
 * specific. 
 *
 * Implementation of some methods rely on certain functions
 * from libcurl, so make sure you have properly installed the 
 * latest version of that library(http://curl.haxx.se/libcurl/).
 *
 * @authors: Daniel Stengerg, Rafayel Mkrtchyan
*/


#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
 
#ifndef CURLPIPE_MULTIPLEX
#define CURLPIPE_MULTIPLEX 0
#endif
 
#define NUM_HANDLES 1000
 
void *curl_hnd[NUM_HANDLES];
int num_transfers;
 
/* a handle to number lookup, highly ineffective when we do many
   transfers... */ 
static int hnd2num(CURL *hnd) {
  int index;
  for(index = 0; index< num_transfers; index++) {
    if(curl_hnd[index] == hnd)
      return index;
  }
  return 0;
}
 
static void dump(const char *text, int num, unsigned char *ptr, size_t size,
          char nohex) {
  size_t i;
  size_t c;
 
  unsigned int width=0x10;
 
  if(nohex)
    /* without the hex output, we can fit more on screen */ 
    width = 0x40;
 
  fprintf(stderr, "%d %s, %ld bytes (0x%lx)\n",
          num, text, (long)size, (long)size);
 
  for(i=0; i<size; i+= width) {
 
    fprintf(stderr, "%4.4lx: ", (long)i);
 
    if(!nohex) {
      /* hex not disabled, show it */ 
      for(c = 0; c < width; c++)
        if(i+c < size)
          fprintf(stderr, "%02x ", ptr[i+c]);
        else
          fputs("   ", stderr);
    }
 
    for(c = 0; (c < width) && (i+c < size); c++) {
      /* check for 0D0A; if found, skip past and start a new line of output */ 
      if (nohex && (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) {
        i+=(c+2-width);
        break;
      }
      fprintf(stderr, "%c",
              (ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.');
      /* check again for 0D0A, to avoid an extra \n if it's at width */ 
      if (nohex && (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A) {
        i+=(c+3-width);
        break;
      }
    }
    fputc('\n', stderr); /* newline */ 
  }
}
 
static int my_trace(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *userp) {
  const char *text;
  int num = hnd2num(handle);
  (void)handle; /* prevent compiler warning */ 
  (void)userp;
  switch (type) {
  case CURLINFO_TEXT:
    fprintf(stderr, "== %d Info: %s", num, data);
  default: /* in case a new one is introduced to shock us */ 
    return 0;
 
  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  }
 
  dump(text, num, (unsigned char *)data, size, 1);
  return 0;
}
 
static void setup(CURL *hnd, int num) {
  FILE *out;
  char filename[128];
 
  sprintf(filename, "dl-%d", num);
 
  out = fopen(filename, "wb");
 
  /* write to this file */ 
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, out);
 
  /* set the same URL */ 
  curl_easy_setopt(hnd, CURLOPT_URL, "http://textfiles.com/100/apples.txt");
 
  /* send it verbose for max debuggaility */ 
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(hnd, CURLOPT_DEBUGFUNCTION, my_trace);
 
  /* HTTP/2 please */ 
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
 
  /* we use a self-signed test server, skip verification during debugging */ 
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
 
#if (CURLPIPE_MULTIPLEX > 0)
  /* wait for pipe connection to confirm */ 
  curl_easy_setopt(hnd, CURLOPT_PIPEWAIT, 1L);
#endif
 
  curl_hnd[num] = hnd;
}
 
/*
 * Simply download two files over HTTP/2, using the same physical connection!
 */ 
int main(int argc, char **argv) {
  clock_t start, end;
  double cpu_time_used;
  start = clock();
     
  CURL *easy[NUM_HANDLES];
  CURLM *multi_handle;
  int i;
  int still_running; /* keep number of running handles */ 
 
  if(argc > 1)
    /* if given a number, do that many transfers */ 
    num_transfers = atoi(argv[1]);
 
  if(!num_transfers || (num_transfers > NUM_HANDLES))
    num_transfers = 3; /* a suitable low default */ 
 
  /* init a multi stack */ 
  multi_handle = curl_multi_init();
 
  for(i=0; i<num_transfers; i++) {
    easy[i] = curl_easy_init();
    /* set options */ 
    setup(easy[i], i);
 
    /* add the individual transfer */ 
    curl_multi_add_handle(multi_handle, easy[i]);
  }
 
  curl_multi_setopt(multi_handle, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
 
  /* we start some action by calling perform right away */ 
  curl_multi_perform(multi_handle, &still_running);
 
  do {
    struct timeval timeout;
    int rc; /* select() return code */ 
    CURLMcode mc; /* curl_multi_fdset() return code */ 
 
    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    int maxfd = -1;
 
    long curl_timeo = -1;
 
    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcep);
 
    /* set a suitable timeout to play around with */ 
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
 
    curl_multi_timeout(multi_handle, &curl_timeo);
    if(curl_timeo >= 0) {
      timeout.tv_sec = curl_timeo / 1000;
      if(timeout.tv_sec > 1)
        timeout.tv_sec = 1;
      else
        timeout.tv_usec = (curl_timeo % 1000) * 1000;
    }
 
    /* get file descriptors from the transfers */ 
    mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
 
    if(mc != CURLM_OK)
    {
      fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
      break;
    }
 
    /* On success the value of maxfd is guaranteed to be >= -1. We call
       select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
       no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
       to sleep 100ms, which is the minimum suggested value in the
       curl_multi_fdset() doc. */ 
 
    if(maxfd == -1) {
#ifdef _WIN32
      Sleep(100);
      rc = 0;
#else
      /* Portable sleep for platforms other than Windows. */ 
      struct timeval wait = { 0, 100 * 1000 }; /* 100ms */ 
      rc = select(0, NULL, NULL, NULL, &wait);
#endif
    }
    else {
      /* Note that on some platforms 'timeout' may be modified by select().
         If you need access to the original value save a copy beforehand. */ 
      rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
    }
 
    switch(rc) {
    case -1:
      /* select error */ 
      break;
    case 0:
    default:
      /* timeout or readable/writable sockets */ 
      curl_multi_perform(multi_handle, &still_running);
      break;
    }
  } while(still_running);
 
  curl_multi_cleanup(multi_handle);
 
  for(i=0; i<num_transfers; i++)
    curl_easy_cleanup(easy[i]);
  
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("\n");
  printf("DOWNLOAD TIME IS ");
  printf("%lf", cpu_time_used);
  printf("seconds");
  printf("\n");
  return 0;
}
