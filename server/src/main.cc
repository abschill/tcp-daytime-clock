#include <ctime>
#include <iostream>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

/*
 * the function make_daytime_string() will create the string to be sent back to the client. 
*/
std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

int main() {
  try {
    asio::io_context io_context;

    /*
     * A ip::tcp::acceptor object needs to be created to listen for new connections. 
     * It is initialised to listen on TCP port 13, for IP version 4.
    */ 
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

    /*
     * This is a iterative/synchronous server, which means that it will handle one connection at a time. 
     * Create a socket that will represent the connection to the client, and then wait for a connection. 
    */
    for (;;) {
      tcp::socket socket(io_context);
      acceptor.accept(socket);
      // create the string when a connection is recieved
      std::string message = make_daytime_string();

      asio::error_code ignored_error;
      // write the buffer to the output from the created string
      asio::write(socket, asio::buffer(message), ignored_error);
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}