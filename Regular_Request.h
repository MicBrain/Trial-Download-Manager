/* "Regular_Request" is the first option of Download Manager. 
 * It allows to download a file through HTTP connection in
 * the simplest but also the slowest way without using the
 * features of pipelining from HTTP/1.1 or multiplexing from
 * HTTP/2.
 * 
 * Implementation of some methods rely on certain functions
 * from libcurl, so make sure you have properly installed the 
 * latest version of that library(http://curl.haxx.se/libcurl/).
 *
 * @author: Rafayel Mkrtchyan
 */

#include <string>
#include <curl/curl.h>

using namespace std;

class Regular_Request {

public:
    Regular_Request(string, int, int);
    Regular_Request(string[], int);
    void setURL(string);
    string getURL();
    double download_single_regular(char*);
    double download_multiple_regular();
    
private:
    string path;
    void getting_started();
    double truncate_double(double, int);
    static size_t write_callback(void*, size_t, size_t, FILE*);
    static void handle_easy_error(CURLcode, string);
};
