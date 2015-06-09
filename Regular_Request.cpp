/* Implementation of the "Regular Request" option of Download
 * Manager. It has two fully-functional constructors - one
 * for downloading just a single file and the other one - for 
 * downloading multiple files.
 *
 * @author: Rafayel Mkrtchyan
*/

#include <math.h>
#include <string>
#include <chrono>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <curl/curl.h>
#include "Regular_Request.h"

using namespace std;
using namespace std::chrono;

/** The first constructor for the Regular Request Manager. 
 * myURL - indicates the link for downloading the file.
 * option - helps to choose either downloading single file or 
 *          multiple files, 0 - single file, 1 - multiple files.
 * message - indicates whether to show the starter message or not.    
 */
Regular_Request::Regular_Request(string myURL, int option, int message) {
    setURL(myURL);
    if (!message)
        getting_started();
    int URL_Length = myURL.length();
    char stringToPass[URL_Length + 1];
    strcpy(stringToPass, myURL.c_str());
    if (option == 0) {
        cout << "Downloading file in ";
        double duration = download_single_regular(stringToPass);
        cout << duration << " seconds" << endl;
    }
}

/** The second constructor for the Regular Request Manager.
 *  URLs - list of the all urls that contain the files for 
 *  the download.
 *  length - length of the URLs array.
 *  This option downloads files using different TCP connec-
 *  tions.
 */
Regular_Request::Regular_Request(string URLs[], int length) {
    int index;
    double overall;
    high_resolution_clock::time_point starting_point = high_resolution_clock::now();
    int URL_Length = URLs[0].length();
    char stringToPass[URL_Length + 1];
    strcpy(stringToPass, URLs[0].c_str());
    Regular_Request(stringToPass, 0, 0);
    for (index = 1; index < length; index++) {
        int URL_Length = URLs[index].length();
        char stringToPass[URL_Length + 1];
        strcpy(stringToPass, URLs[index].c_str());
        Regular_Request(stringToPass, 0, 1);
    }
    high_resolution_clock::time_point ending_point = high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::microseconds>( ending_point- starting_point).count();
    overall = truncate_double(duration/1000000, 4);
    cout << "Downloading all the files in " << overall << " seconds" << endl;
}

/** The third constructor for the Regular Request Manager.
 *  URLs - list of the all urls that contain the files for 
 *  the download.
 *  length - length of the URLs array.
 *  keepAlive - option for HTTP keep-alive functionality.
 *  This option downloads files using single TCP connection.
 */
Regular_Request::Regular_Request(string URLs[], int length, bool keepAlive) {
    double result;
    string message = "curl_easy_setopt";
    if (keepAlive) {
        double overall;
        high_resolution_clock::time_point start = high_resolution_clock::now();
        CURL *session;
        CURLcode option;
        CURLcode curl_code;
        session = curl_easy_init();
        if (session) {
            FILE *devnull = fopen("/dev/null", "w+");
            for (int index = 0; index < length; index++) {
                int URL_Length = URLs[index].length();
                char stringToPass[URL_Length + 1];
                strcpy(stringToPass, URLs[index].c_str());
                option = curl_easy_setopt(session, CURLOPT_URL, stringToPass);
                handle_easy_error(option, message);
                option = curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, write_callback);
                handle_easy_error(option, message);
                option = curl_easy_setopt(session, CURLOPT_WRITEDATA, devnull);
                handle_easy_error(option, message);
                curl_code = curl_easy_perform(session);
            }
            fclose(devnull);
            curl_easy_cleanup(session);
        }
        high_resolution_clock::time_point end = high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>( end- start ).count();
        result = truncate_double(duration/1000000, 4);
        cout << "Downloading files in " << result << " seconds" << endl;
    }
}

/* The setter method. */
void Regular_Request::setURL(string myURL) {
    path = myURL;
}

/* The getter method. */
string Regular_Request::getURL() {
    return path;    
}

/* Generates the functionality for writing received data. */
 size_t Regular_Request::write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

/* Downloads one file using HTTP connection. */
double Regular_Request::download_single_regular(char* url) {
  double result;
  string message = "curl_easy_setopt";
  high_resolution_clock::time_point start = high_resolution_clock::now();
  CURL *session;
  CURLcode option;
  CURLcode curl_code;
  session = curl_easy_init();
  if (session) {
    FILE *devnull = fopen("/dev/null", "w+");
    option = curl_easy_setopt(session, CURLOPT_URL, url);
    handle_easy_error(option, message);
    option = curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, write_callback);
    handle_easy_error(option, message);
    option = curl_easy_setopt(session, CURLOPT_WRITEDATA, devnull);
    handle_easy_error(option, message);
    curl_code = curl_easy_perform(session);
    fclose(devnull);
    curl_easy_cleanup(session);
  }
  high_resolution_clock::time_point end = high_resolution_clock::now();
  double duration = std::chrono::duration_cast<std::chrono::microseconds>( end- start ).count();
  result = truncate_double(duration/1000000, 4);
  return result;
}

/* Initial message before running Regular Request option. */
void Regular_Request::getting_started() {
    cout << "You are running the <Regular Request> option of Download Manager." <<endl;
    cout << "Developed by Rafayel Mkrtchyan: rafamian@berkeley.edu" << endl;
    return;
}

/** Handles errors occuring because of setting unacceptable options 
 *  for curl session. 
 */
void Regular_Request::handle_easy_error(CURLcode ret, string message) {
  if(ret != CURLE_OK){
      cerr <<"Error: Fatal problem has occured with " << message << \
            "functionality" << endl;
  }
  return;
}

/* Truncates VALUE up to POSITIONS decimal places after decimal point. */
double Regular_Request::truncate_double(double value, int positions)
{
    double temporary = 0.0;
    temporary = (int) (value * pow(10, positions));
    temporary /= pow(10, positions);
    return temporary;
}
