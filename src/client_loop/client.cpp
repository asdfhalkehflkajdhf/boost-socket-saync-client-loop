#include "client_async_loop.h"
using namespace std; // For strlen and memcpy.

int main(int argc, char* argv[])
{

    string ip="127.0.0.1";
    int port=6688;
	
	cout<<"============================="<<endl;
	cout<<"para <is user msg qeueu[y/n]> [ip] [port]"<<endl;
	cout<<"para <is user msg qeueu[y/n]> [port]"<<endl;
	cout<<"para <is user msg qeueu[y/n]> default "<<ip<<":"<<port<<endl;
	cout<<"============================="<<endl;

    boost::asio::io_service io_service_;
	
	if(argc<2)
		exit(1);
	
	client_send_queue csq = (argv[1][0]=='y' || argv[1][0]=='Y' )?client_send_queue::CSQ_ON:client_send_queue::CSQ_OFF;
	
	if (argc ==3)
	{
		port = atoi(argv[2]);
	}else if(argc == 4){
		ip=argv[2];
		port = atoi(argv[3]);
	}else{
		exit(1);
	}
	
	std::cout<<"cur ["<<ip<<":"<<port<<"] is user qeueu["<<argv[1]<<"]"<<endl;

    try
    {
        //client_loop c(io_service_, string(argv[1]), atoi(argv[2]));
		
        client_loop c(io_service_, string(ip), port, csq);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &c.io_service_));

        const int max_body_length=1024;
        char line[max_body_length];

        while (std::cin.getline(line, max_body_length))
        {
            if(c.is_open() == false){
                std::cout<<"\tconnect is close"<<std::endl;
                continue;
            }
			if(strcmp(line, "exit") == 0)
				break;

            int reqid = c.write(line, strlen(line)+1);
            cout<<"c reqid="<<reqid<<endl;
            auto recvRes = c.read(reqid);
            if(recvRes == nullptr)
                cout<<"c read error"<<endl;
            else if(recvRes->get_status() != CT_READ_OK)
				cout<<"c read error "<<recvRes->get_status()<<endl;
			else
                cout<<"c read ok :"<<recvRes->body()<<" | "<<recvRes->head().body_len()<<endl;
        }

        c.close();
		std::cout<<"main exit s"<<std::endl;
		io_service_.stop();
        t.join();
		std::cout<<"main exit t"<<std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
