# Trial Download Manager

### What is Download Manager?

  "Download Manager" is a functional software application that allows users to play with three various choices for requesting files from the server. Those choices are without any fancy attributes, using HTTP/1.1 technology and using HTTP/2 technology. This package is an experimental collection of C++ and C programs that helps us to test the speed of downloading various files using HTTP connection. This is part of "GSoC CernVM-FS" project.
  
### How to Run

I have made a Makefile in order to make it easier to compile and run the project. In order to compile and run, enter these following commands: 

```
make      # compiler the whole package
```

### Software Requirements

  Currently this software is guaranteed to run perfectly on Ubuntu OS. During the development of this project, the official version of libcurl that supports HTTP/2 was not from the git repo: https://github.com/bagder/curl. The next version (7.43.0) of libcurl will become the first official version to support multiplexed transfers over HTTP/2. Also note that you then need nghttp2 version 1.0.0 or later. In order to test "Multiplexed Request" functionality of this Download Manager you need to create an operational server that supports HTTP/2 protocol. Some recommended HTTP/HTTPS servers that support HTTP/2 are:
  
  * OpenLiteSpeed 1.3.11 and 1.4.8
  * LiteSpeed Web Server 5.0
  * Akamai Edge Servers support
  
