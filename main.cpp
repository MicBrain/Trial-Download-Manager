/***************************************************************************
 * Project
 *                          DOWNLOAD MANAGER
 * 
 * This experimental software application is designed in order to compare
 * the speeds of three options of requesting files - Regular, Pipelined using
 * HTTP/1.1 protocol and Multiplexed using HTTP/2 technology. I am using the
 * most recent versions of libcurl and nghttp2 packages.
 * 
 * You may opt to use, copy, modify, merge, publish, distribute copies of 
 * the this project, and permit persons to whom the Software is furnished
 * to do so, under the terms of the Apache license.
 *
 * @author: Rafayel Mkrtchyan
*/

#include <math.h>
#include <chrono>
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <curl/curl.h>
#include "Regular_Request.h"
#include "Pipelined_Request.h"

using namespace std;
using namespace std::chrono;

int main(void) {
  string sample_URL = "http://textfiles.com/100/914bbs.txt";
  string URLs[] = {sample_URL, sample_URL, sample_URL, sample_URL, sample_URL};
  
  cout << "### REGULAR REQUEST ###" << endl;
  cout << "Downloading just one file" << endl;
  Regular_Request regular = Regular_Request(sample_URL, 0, 0);
  cout << "Downloading multiple files" << endl;
  Regular_Request regular2 = Regular_Request(URLs, 2);
  
  cout << "### PIPELINED REQUEST ###" << endl;
  cout << "Downloading multiple files" << endl;
  Pipelined_Request piped = Pipelined_Request();
  piped.download_multiple_pipelined(URLs, 2);
  
  return 0;
}

