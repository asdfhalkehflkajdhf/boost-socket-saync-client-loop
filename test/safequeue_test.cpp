
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include "safequeue.hpp"
using namespace std;

#define counts 2000
#define countsTime 50

#define wthreadNum 4            //写线程个数
#define rthreadNum 4            //读线程个数
#define ethreadNum 4            //擦拭线程个数


queue<int> stdqu;
void test_stdqu(){

    //启动
    pthread_t _theadId[wthreadNum + rthreadNum + ethreadNum];

    for(int i=0; i<wthreadNum + rthreadNum + ethreadNum ; i++)
    {
        if(i < wthreadNum)
        {//write
            pthread_create(&_theadId[i], NULL, [](void *args)->void* {

                //    while(1)
                for(int j=0; j<countsTime; j++)
                {
                    for(int i=0; i<counts; i++)
                    {
                        stdqu.push(i);

                    }
                    usleep(1);
                }

                return NULL;
            }, NULL);
        }
        else if(i < wthreadNum + rthreadNum)
        {//read
            pthread_create(&_theadId[i], NULL, [](void *args)->void* {

                //    while(1)
                for(int j=0; j<countsTime; j++)
                {
                    if(!stdqu.empty())
                        stdqu.front();

                    usleep(2);

                }

                return NULL;
            }, NULL);
        }
        else if(i < wthreadNum + rthreadNum + ethreadNum)
        {//erase
            pthread_create(&_theadId[i], NULL, [](void *args)->void* {

                //    while(1)
                for(int j=0; j<countsTime; j++)
                {
                    for(int i=0; i<counts; i++)
                    {

                        stdqu.pop();
                        usleep(2);

                    }

                }

                return NULL;
            }, NULL);
        }

    }
    pthread_join(_theadId[wthreadNum + rthreadNum + ethreadNum ], NULL);

    std::cout<<stdqu.empty()<<std::endl;
}


std::atomic_int add_t=ATOMIC_VAR_INIT(0);
std::atomic_int sub_t=ATOMIC_VAR_INIT(0);

SafeQueue<int> squ;
void test_squ(){

    //启动
    pthread_t _theadId_w[wthreadNum];
    pthread_t _theadId_r[ rthreadNum];

    for(int i=0; i<wthreadNum; ++i)
        pthread_create(&_theadId_w[i], NULL, [](void *args)->void* {

            //    while(1)
                for(int i=0; i<counts; i++)
                {
                    squ.push(i);
                    add_t.fetch_add(1, std::memory_order_release);

                }
                usleep(1);

            return NULL;
        }, NULL);


    for(int i=0; i<rthreadNum; ++i)
        pthread_create(&_theadId_r[i], NULL, [](void *args)->void* {

            //    while(1)
                for(int i=0; i<counts; i++)
                {
                    squ.pop();
                    sub_t.fetch_add(1, std::memory_order_release);

                }
                usleep(1);

            return NULL;
        }, NULL);

    for(int i=0; i<wthreadNum; ++i)
        pthread_join(_theadId_w[i], NULL);

    for(int i=0; i<rthreadNum; ++i)
        pthread_join(_theadId_r[i], NULL);

    std::cout<<"add t="<<add_t<<std::endl;
    std::cout<<"sub t="<<sub_t<<std::endl;

    std::cout<<squ.add_size_<<std::endl;
    std::cout<<squ.sum_size_<<std::endl;
    std::cout<<squ.size_<<std::endl;
}



int main()
{

    //test_stdqu();
    test_squ();
    return EXIT_SUCCESS;
}
