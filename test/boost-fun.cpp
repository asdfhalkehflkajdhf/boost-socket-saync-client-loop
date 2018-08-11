#include <boost/function.hpp>
#include <iostream>

using namespace std;

void do_sum(int *values, int n) 
{ 
    int sum(0);
    for (int i = 0; i < n; ++i) 
    { 
        sum += values[i]; 
    } 
    cout << sum << endl; 
};


class test1{
	boost::function<void(int *values, int n)> sum; 
public:
	void register_callback(boost::function<void(int *values, int n)> f){
		sum = f;//boost::move(f); 
	}
	~test1(){
		int a[] = {1,2,3};
		sum(a,3);
	}
};

class test2{
	boost::function<void(int *values, int n)> sum;
	test1 t1;
public:
	void register_callback(boost::function<void(int *values, int n)> f){
		sum = f;//boost::move(f); 
		t1.register_callback(sum);
	}
	~test2(){
		int a[] = {1,2,3,4};
		sum(a,4);
	}
};

int main(int argc, char* argv[]) 
{ 
    boost::function<void(int *values, int n)> sum; 
	if(sum)
		std::cout<<"t"<<std::endl;
	else
		std::cout<<"f"<<std::endl;
    sum = &do_sum;
		if(sum)
		std::cout<<"t"<<std::endl;
	else
		std::cout<<"f"<<std::endl;

    int a[] = {1,2,3,4,5}; 
    sum(a, 5);
	
	test2 t2;
	t2.register_callback(&do_sum);
	
    return 0; 
}