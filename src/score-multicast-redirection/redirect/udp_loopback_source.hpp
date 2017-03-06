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
class udp_loopback_source
{
    struct mylink : public links::link
    {
        using link::link;
        using link::async_receive;
    };

public:
    udp_loopback_source(uint16_t port) :
        m_io(),
        m_link(m_io),
        m_recv_buffer(m_max_buffer_size)
    {
        m_link.bind(boost::asio::ip::address_v4::loopback(), port);

        m_link.async_receive(m_recv_buffer,
                             std::bind(&udp_loopback_source::receive_handler,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));

        m_thread = std::thread([&]() { m_io.run(); });
    }

    ~udp_loopback_source()
    {
        m_io.stop();
        m_thread.join();
    }

    void set_receive_callback(
        const std::function<void(const std::vector<uint8_t>&)>& callback)
    {
        m_receive_callback = callback;
    }

private:

    void receive_handler(const std::error_code& error, uint32_t bytes)
    {
        if (error)
        {
            return;
        }
        m_recv_buffer.resize(bytes);

        if (m_receive_callback)
        {
            m_receive_callback(m_recv_buffer);
        }

        m_recv_buffer.resize(m_max_buffer_size);

        m_link.async_receive(m_recv_buffer,
                             std::bind(&udp_loopback_source::receive_handler,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
    }

private:

    boost::asio::io_service m_io;

    mylink m_link;

    std::thread m_thread;

    std::function<void(const std::vector<uint8_t>&)> m_receive_callback;

    const uint32_t m_max_buffer_size = 66000;
    std::vector<uint8_t> m_recv_buffer;
};
}
