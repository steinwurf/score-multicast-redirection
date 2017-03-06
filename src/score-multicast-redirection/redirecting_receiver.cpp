// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <iostream>
#include <vector>

#include <scorecpp/scorecpp.hpp>

#include "redirect/udp_loopback_destination.hpp"

// Redirects a data stream from score (multicast) to a local port
// Example to be used with redirecting sender.
// Resulting RTP stream can be viewed in vlc with the following command:
//
//      vlc rtp://127.0.0.1:23337
//
// NB: This example does not provide any power-save inhibiting features

int main(int argc, char* argv[])
{
    std::string addr = "0.0.0.0";

    if (argc > 2)
    {
        std::cout << "Usage: " << argv[0] << "[ip_address]" << std::endl;
        return -1;
    }

    if (argc == 2)
    {
        addr = argv[1];
    }

    // Create the io_service object
    scorecpp::io_service io;

    // Create the score receiver object
    scorecpp::receiver receiver(io);

    std::error_code ec;
    // Bind the score receiver to a specific address and port
    // The address can be a local IPv4 address or a multicast/broadcast address
    // It can also be "0.0.0.0" to listen on all local interfaces
    receiver.bind(addr, 7891, ec);

    if (ec)
    {
        std::cerr << "Could not bind score receiver. Error: " << ec.message()
                  << std::endl;
        return ec.value();
    }

    // Setup the udp loopback redirection to send messages to port 13337 on
    // the loopback interface
    redirect::udp_loopback_destination destination(23337);

    // Set messages received by score to be sent by to redirection destination.
    auto read_data = [&](const uint8_t* buffer, uint32_t size)
    {
        destination.send(buffer, size);
    };

    // Set the callback to be used when data is received
    receiver.set_data_ready_callback(read_data);

    // Run the event loop of the io_service
    io.run();

    // The event loop will run until the app is terminated

    return 0;
}
