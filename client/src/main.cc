//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <boost/array.hpp>
#include <asio.hpp>

using asio::ip::tcp;

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    /*
     * We need to setup an i/o context 
    */
    asio::io_context io_context;

    /*
     * We need to turn the parameter name from the user into a tcp endpoint 
     * using the tcp::resolver obj
    */
    tcp::resolver resolver(io_context);

    /*
     * It takes a hostname and service name and turns it into a list of endpoints
     * we first perform the resolve call using the name of the server with the service name
     * "daytime". 

     * The return type of the resolution is an ip::tcp::resolver::results_type,
     * which is a range used for iterating over the results.

     * The range object comes with begin() and end() member functions for 
     * the aforementioned iteration
    */
    tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");

    /*
     * Following that, we create and connect to the socket. our endpoints above may contain both
     * ipv4 and ipv6 endpoints

     * The asio::connect method will automatically find one that works for us,
     * which keeps our client program independent of a specific IP version
    */
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    /*
     * Once the connection is open, we just need to read the response from the service 

     * we use boost::array to hold the data recieved, and asio::buffer to determine the size
     * of the array, which helps us to protect against buffer overrun, which is distinct from a 
     * buffer overflow, a buffer overflow is when you put more items than the buffer can hold,
     * but a buffer overrun is when you are iterating over the boundary past the end of it's length
    */

    // THE CRYING FOR LOOP MAKES AN APPEARANCE
    for (;;) {

      // just like boost::array, good old char[] or std::vector could work here too
      boost::array<char, 128> buf;
      asio::error_code error;

      size_t len = socket.read_some(asio::buffer(buf), error);

      if (error == asio::error::eof) {
        break; // Connection closed cleanly by peer.
      }
        
      else if (error) {
        throw asio::system_error(error); // Some other error.
      }
       
      std::cout.write(buf.data(), len);
    }
  } // handle any exceptions
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}