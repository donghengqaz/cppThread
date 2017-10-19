#include <iostream>

#include "message.hpp"
#include "thread.hpp"
#include "mutex.hpp"
#include "socket.hpp"
#include "io.hpp"

using namespace estd;

static class mutex s_mutex;
static class message<int, 10> s_msg;
static class message<char [32], 10> s_msg2;

void te1(int c, int p)
{
    char num = 0;

    while (c--) {
        s_mutex.lock(56.32);
        std::cout << "1 " << p++ << std::endl;
        s_mutex.unlock();
        s_msg.send(p);
        char bd[32] = "A234567890";
        bd[0] = bd[0] + num;
        s_msg2.send(bd);
        num = (num + 1) % 10;
        sleep(1.1);
    }
}

void te2(int c, std::string &p)
{
    while (c--) {
        auto pp = 0;
        s_msg.recv(pp);
        s_mutex.lock(33.22);
        std::cout << "2 " << p << " " << pp << std::endl;
        s_mutex.unlock();
        sleep(1.1);
    }
}

void te3(estd::time_t t)
{
    try {
        class socket s0;
        class socket s1(SOCK_DGRAM, 5050);
        class socket s2(SOCK_DGRAM, 5051);
        class socket s3(SOCK_DGRAM, 5052);

        class socket s4(SOCK_STREAM, 5053, true);
        class socket s5(SOCK_STREAM, 5054, true);
        class socket s6(SOCK_STREAM, 5055, true);

        class iolist rl;
        class iolist wl; // just not use
        class iolist el; // just not use

        std::cout << "connect " << s4.connect(estd::ipaddr("www.baidu.com", 80)) << std::endl;
        //std::cout << "connect " << s5.connect(estd::ipaddr("192.168.1.105", 5050)) << std::endl;

        rl += s1;
        rl += s2;
        rl += s3;
        rl += s_msg2;
        rl += s6;
        rl += s0;

        s6.listen(1);
        s6.fcntl(true, O_NONBLOCK);

        while (1) {
            el = rl;

            auto cap = estd::select(rl, wl, el, t);

            if (std::get<0>(cap) <= 0) {
                std::cout << "3 ret " << std::get<0>(cap) << std::endl;
                break;
            }

            for (auto &c:std::get<3>(cap)) {
                std::cout << "error " << c.fd() << std::endl;
                rl -= c;
                c.close();
            }

            for (auto &c:std::get<2>(cap))
                std::cout << "write " << c.fd() << std::endl;

            for (auto &c:std::get<1>(cap)) {
                if (s6 == c) {
                    std::cout << "accept " << c.fd() << std::endl;
                    auto s = s6.accept();
                    s.write("cppThread accept");
                    s0 = s;
                } else {
                    std::cout << "read " << c.fd() << std::endl;
                    auto bd = c.read();
                    if (bd.size() > 0) {
                        bd.data()[bd.size()] = 0;
                        s_mutex.lock(33.22);
                        std::cout << "3 fd: " << c.fd() << " data: " << bd.data() << std::endl;
                        s_mutex.unlock();
                        if (bd.data()[0] == 'E')
                            rl -= c;
                    } else {
                        std::cout << "close fd " << c.fd() << std::endl;
                        rl -= c;
                        c.close();
                    }
                }
            }

            if (!rl.size())
                goto exit;
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

exit:
    std::cout << "te3 exit" << std::endl;
}

static void main_test(void)
{
    int a1 = -100;
    std::string a2 = "-200";

    class thread<> t1(te1, 500, a1);
    class thread<8192, 4> t2(te2, 15, a2);
    class thread<8192, 5> t3(te3, 5.1);

    sleep(15.1);
}

extern "C" int main(void)
{
    while (1)
        main_test();

    return 0;
}
