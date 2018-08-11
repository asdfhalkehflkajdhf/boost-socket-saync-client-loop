


#include <iostream>

#include "client_async_loop_pool.h"

using namespace std;

string ip="127.0.0.1";
std::vector<int> port_list;
client_loop_pool &c = client_loop_pool::get_instance();


void single_test(int pid){

    const int max_body_length=1024;
    char line[max_body_length];
    char *recv;
    int port_index=0;

    string thread_id=to_string(pid);

    try
    {
        while (std::cin.getline(line, max_body_length))
        {
            using namespace std; // For strlen and memcpy.
            auto handle = c.write(ip, port_list[(port_index++)%port_list.size()], line, strlen(line)+1);
            recv = c.read(handle);
            if(recv == nullptr)
                cout<<"cp read error"<<endl;
            else
                cout<<"cp read ok :"<<recv<<endl;
        }

    }
    catch (std::exception& e)
    {
          std::cerr << "Exception: " << e.what() << "\n";
    }
}


const int sendTime=200;
const int pthreadNum=4;
void pthread_fun(int pid){

    const int max_body_length=1024;
    char line[max_body_length]={0};
    char *recv=nullptr;
    int port_index=0;

    string thread_id=to_string(pid);

    try
    {
        for(int i=0;i<sendTime; ++i)
        {
            string sendMsg = thread_id+" hello server "+to_string(i);

            memcpy(line,sendMsg.c_str(), sendMsg.length());
            auto handle = c.write(ip, port_list[(port_index++)%port_list.size()], line, strlen(line)+1);
            recv = c.read(handle);
            if(recv == nullptr)
                cout<<"cp read error"<<endl;
            else
                cout<<"cp read ok :"<<recv<<endl;

        }
        cout<<"\t\t"<<thread_id<<" exit"<<endl;
    }
    catch (std::exception& e)
    {
          std::cerr << "Exception: " << e.what() << "\n";
    }
}

void pthread_test(){

	boost::thread *t[pthreadNum];

    for(int i=0; i<pthreadNum; ++i){
        t[i]=new boost::thread(pthread_fun, i);
    }

    for(int i=0; i<pthreadNum; ++i){
        t[i]->join();
		cout<<"join "<<i<<endl;
		delete t[i];
    }

}


int main(int argc, char* argv[])
{

    if (argc < 3)
    {
      std::cerr << "Usage: link_server s <port> [<port> ...]\n";
      std::cerr << "Usage: link_server m <port> [<port> ...]\n";
      return 1;
    }


    for (int i = 2; i < argc; ++i)
    {
        port_list.push_back(atoi(argv[i]));
    }

    switch(argv[1][0]){
        case 's':
        case 'S':
            single_test(1);
            break;
        case 'm':
        case 'M':
            pthread_test();
            break;
        default:
            std::cerr<<"opt error"<<endl;
            break;
    }
	cout<<"\t\tmain exit"<<endl;
    return 0;
}
