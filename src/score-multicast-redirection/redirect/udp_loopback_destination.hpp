// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <thread>

#include <boost/asio.hpp>

#include <links/link.hpp>

namespace redirect
{
class udp_loopback_destination
{
    struct mylink : public links::link
    {
        using link::link;
        using link::send;
    };

public:
    // Create an udp loopback redirecter that sends data buffer to a
    // port on the loopback interface
    udp_loopback_destination(uint16_t port) :
        m_io(),
        m_link(m_io)
    {
        m_link.add_remote(boost::asio::ip::address_v4::loopback(), port);

        m_thread = std::thread([&]() { m_io.run(); });
    }

    ~udp_loopback_destination()
    {
        m_io.stop();
        m_thread.join();
    }

    void send(const uint8_t* data, uint32_t size)
    {
        message_format msg(data, size);
        m_link.send(msg);
    }

private:
    // Internal message format class needed to use links API
    struct message_format
    {
        message_format(const uint8_t* data, uint32_t size) :
            m_data(data, data + size)
        {
        }

        void write(std::vector<uint8_t>& v) const
        {
            v = m_data;
        }

        uint32_t size_serialized() const
        {
            return m_data.size();
        }

    private:
        std::vector<uint8_t> m_data;
    };

private:
    // The internal event loop
    boost::asio::io_service m_io;

    mylink m_link;

    // The thread running the loopback redirection;
    std::thread m_thread;
};
}
