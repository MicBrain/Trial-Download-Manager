/* Implementation of the "Pipelined Request" option of Download
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
#include <iostream>
#include "Pipelined_Request.h"

using namespace std;
using namespace std::chrono;

/* The first constructor for the Pipelined Request Manager.
 * url - indicates the link for downloading the file.
 * firstCall - indicates whether to show the starter message 
 *             or not.
 */
Pipelined_Request::Pipelined_Request(string url, int firstCall) {
    if (!firstCall) {
        welcome();
    }
    setURL(url) ;
    int URL_Length = url.length();
    char stringToPass[URL_Length + 1];
    strcpy(stringToPass, url.c_str());
    double duration = download_single_pipelined(stringToPass);
    cout << "Downloading file in " << duration << " seconds" << endl;
}

/* The second constructor for the Pipelined Request Manager.
 * Use this constructor if you are trying to download multiple 
 * files by just one call.
 * Look at download_multiple_pipelined function if you want to
 * use the option of downloading multiple files.
 */
Pipelined_Request::Pipelined_Request() {
    welcome();
}

/* Downloads one file using the pipelining functionality of 
 * HTTP/1.1 technology
*/
double Pipelined_Request::download_single_pipelined(char *url) {
    int index = 0;
    high_resolution_clock::time_point start = high_resolution_clock::now();
    CURLM *m_curl;
    CURLMcode res;
    m_curl = curl_multi_init();
    curl_multi_setopt(m_curl, CURLMOPT_PIPELINING, 1L);
    CURL *curl[32];
    for(index = 0; index < 1; index++) {
        curl[index] = curl_easy_init();
        if(!curl[index]) {
            cout << "Something went wrong" << endl;
        }
        curl_easy_setopt(curl[index], CURLOPT_URL, url);
        curl_easy_setopt(curl[index], CURLOPT_FOLLOWLOCATION, 1L);
        curl_multi_add_handle(m_curl, curl[index]);
    }
    int ret = 1;
    do {
        res = curl_multi_perform(m_curl, &ret);
        
    } while(ret);
    
    for (index = 0; index < 1; index++) {
        curl_multi_remove_handle(m_curl, curl[index]); 
        curl_easy_cleanup(curl[index]);
    }
    high_resolution_clock::time_point end = high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::microseconds>( end- start ).count();
    double result = truncate_double(duration/1000000, 4);
    return result;
}

/* Downloads multiple files using multiplexing functionality. URLs is the array
 * of the urls of the files that we need to download. LENGTH is the lengths of 
 * that array.
*/
double Pipelined_Request::download_multiple_pipelined(string URLs[], int length) {
    int index;
    high_resolution_clock::time_point start = high_resolution_clock::now();
    CURLM *m_curl;
    CURLMcode res;
    m_curl = curl_multi_init();
    for (index = 0; index < length; index++) {
        curl_multi_setopt(m_curl, CURLMOPT_PIPELINING, 1L);
        int URL_Length = URLs[index].length();
        char stringToPass[URL_Length + 1];
        strcpy(stringToPass, URLs[index].c_str());
       
        CURL *curl[32];
        for(int i=0; i < 1; i++) {
            curl[i] = curl_easy_init();
            if(!curl[i]) {
                cout << "Something wrong" << endl;
            }
            curl_easy_setopt(curl[i], CURLOPT_URL, stringToPass);
            curl_easy_setopt(curl[i], CURLOPT_FOLLOWLOCATION, 1L);
            curl_multi_add_handle(m_curl, curl[i]);
        }
        int ret = 1;
        do {
            res = curl_multi_perform(m_curl, &ret);
        } while(ret);
        for (int i = 0; i < 1; i++) {
            curl_multi_remove_handle(m_curl, curl[i]); 
            curl_easy_cleanup(curl[i]);
        }
   }
  curl_multi_cleanup(m_curl);
  high_resolution_clock::time_point end = high_resolution_clock::now();
  double duration = std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();
  double result = truncate_double(duration/1000000, 4);
  cout << "Downloading files in " << result << " seconds" << endl;
  return result;
}

/* The setter method. */
void Pipelined_Request::setURL(string myURL) {
    path = myURL;
}

/* The getter method. */
string Pipelined_Request::getURL() {
    return path;
}

/* Initial message before running Pipelined Request option. */
void Pipelined_Request::welcome() {
    cout << "You are running the <Pipelined Request> option of Download Manager." <<endl;
    cout << "Developed by Rafayel Mkrtchyan: rafamian@berkeley.edu" << endl;
    return;
}

/* Truncates VALUE up to POSITIONS decimal places after decimal point. */
double Pipelined_Request::truncate_double(double value, int positions)
{
    double temporary = 0.0;
    temporary = (int) (value * pow(10, positions));
    temporary /= pow(10, positions);
    return temporary;
}

/** Handles errors occuring because of setting unacceptable options 
 *  for curl session. 
 */
void Pipelined_Request::handle_multi_error(CURLcode ret, string problem) {
  if(ret != CURLE_OK){
      cerr <<"Problem has occured with " << problem << \
            "function." << endl;
  }
  return;
}
