#include "client_async_loop_no_queue.h"
using namespace std; // For strlen and memcpy.

int main(int argc, char* argv[])
{

    string ip="127.0.0.1";
    int port=6688;

    boost::asio::io_service io_service_;

    try
    {
        if (argc ==2)
        {
            ip=argv[1];
        }else if(argc >= 3){
            ip=argv[1];
            port = atoi(argv[2]);
        }
        std::cout<<ip<<":"<<port<<endl;


        //client_loop c(io_service_, string(argv[1]), atoi(argv[2]));
        client_loop c(io_service_, string(ip), port);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &c.io_service_));

        const int max_body_length=1024;
        char line[max_body_length];

        while (std::cin.getline(line, max_body_length))
        {
            if(c.is_open() == false){
                std::cout<<"\tconnect is close"<<std::endl;
                continue;
            }

            int reqid = c.write(line, strlen(line)+1);
            cout<<"c reqid="<<reqid<<endl;
            auto recvRes = c.read(reqid);
            if(recvRes == nullptr || recvRes->get_status() != CT_READ_OK)
                cout<<"c read error"<<endl;
            else
                cout<<"c read ok :"<<recvRes->body()<<endl;
        }

        c.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
