/* "Pipelined_Request" is the second option of our Download Manager. 
 * This class allows to download file(s) using pipelining 
 * functionality of HTTP/1.1 technology. Generally it is
 * faster than "Regular_Request".
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

class Pipelined_Request {

public:
    Pipelined_Request(string, int);
    Pipelined_Request();
    void setURL(string);
    string getURL();
    double download_single_pipelined(char*);
    double download_multiple_pipelined(string[], int);

private:
    string path;
    void welcome();
    double truncate_double(double, int);
    static void handle_multi_error(CURLcode, string);
};