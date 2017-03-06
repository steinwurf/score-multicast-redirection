// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <iostream>
#include <vector>

#include <scorecpp/scorecpp.hpp>

#include "redirect/udp_loopback_source.hpp"

// This example redirects traffic from a local socket to a multicast address
// and wraps it in the score protocol.
// Create a local RTP stream e.g. with the following vlc command:
//
//      vlc [VIDEO_FILE] --sout '#duplicate{dst=display,
//          dst=rtp{mux=ts,dst=127.0.0.1,port=13337}}'
//
// where [VIDEO_FILE] is a path to a video file

// NB: This example does not provide any power-save inhibiting features

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage: " << argv[0] << " remote_ip [interface_ip]"
                  << std::endl;
        return -1;
    }

    // Create the io_service object
    scorecpp::io_service io;

    // Create the score sender object
    scorecpp::sender sender(io, scorecpp::sender_profile::stream);

    // Set the send rate to something quite high
    sender.set_send_rate(uint32_t(10e6)); // 10MB/s should be sufficient

    std::error_code ec;
    // Configure the destination address and port on the score sender.
    // Address can be any IPv4 address, including multicast and broadcast.
    // Set remote to a multicast address and port. The third argument is the
    // local interface to use for multicast, and can be omitted to use the
    // default interface.
    // The third argument should be modified to the correct interface
    sender.add_remote("224.0.0.251", 7891, ec);

    if (ec)
    {
        std::cout << "Error setting remote address: " << ec.message()
                  << std::endl;
        return ec.value();
    }

    if (argc == 3)
    {
        sender.set_interface_multicast(argv[2], ec);

        if (ec)
        {
            std::cout << "Error setting multicast interface: " << ec.message()
                      << std::endl;
            return ec.value();
        }
    }

    // At this point the score sender is ready to send data over the network.

    // Now we configure the redirection socket
    // We redirect packets from port 13337:
    redirect::udp_loopback_source source(13337);

    // Set source to add all received packets to score sender
    source.set_receive_callback([&](std::vector<uint8_t> data)
    {
        sender.write_data(data.data(), data.size());
    });

    // Run the event loop of the io_service
    io.run();

    // The event loop will run until the app is terminated

    return 0;
}
