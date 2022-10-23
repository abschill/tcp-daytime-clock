#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection> {
public:
  /*
   * We will use shared_ptr and enable_shared_from_this because we want to keep the tcp_connection object alive 
   as long as there is an operation that refers to it.
  */ 
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_context& io_context) {
    return pointer(new tcp_connection(io_context));
  }

  tcp::socket& socket() {
    return socket_;
  }

  void start() {
    /**
     * he data to be sent is stored in the class member message_ 
     * because we need to keep the data valid until the asynchronous operation is complete.
    */
    message_ = make_daytime_string();
    /*
     * we call boost::asio::async_write() to serve the data to the client. Note that we are using boost::asio::async_write(), 
     * rather than ip::tcp::socket::async_write_some(), to ensure that the entire block of data is sent.
     * if using boost::bind(), you must specify only the arguments that match the handler's parameter list. 
     * in this case, both of the argument placeholders (boost::asio::placeholders::error and boost::asio::placeholders::bytes_transferred) 
     could potentially have been removed, since they are not being used in handle_write()
    */
    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    /*
     * further actions for this client connection are now the responsibility of handle_write()
    */
  }

private:
  tcp_connection(boost::asio::io_context& io_context)
    : socket_(io_context) {
  }

  // this is the destination of where those unused params in theory could have been removed,
  // because as seen here we are not doing anything with them but we could
  void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/) {
  }

  tcp::socket socket_;
  std::string message_;
};

class tcp_server {
public:
  tcp_server(boost::asio::io_context& io_context)
    : io_context_(io_context),
      acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)) { // once again we post up on port 13
    start_accept(); // this will start the async accept operation to wait for a new connection
  }

private:
  void start_accept() {
    tcp_connection::pointer new_connection =
      tcp_connection::create(io_context_);

    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection,
          boost::asio::placeholders::error));
  }

  /*
   * handle_accept will be called when the async accept operation is initiated by the incoming connection
   * It services the client request, and then calls start_accept() to initiate the next accept operation.
  */
  void handle_accept(tcp_connection::pointer new_connection,
      const boost::system::error_code& error) {
    if (!error) {
      new_connection->start();
    }

    start_accept();
  }

  boost::asio::io_context& io_context_;
  tcp::acceptor acceptor_;
};

int main() {
  try {
    boost::asio::io_context io_context;
    tcp_server server(io_context);
    // Run the io_context object so that it will perform asynchronous operations on your behalf.
    io_context.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}