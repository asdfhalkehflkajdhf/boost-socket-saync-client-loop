
//============================================================================
// Name        : TestWDMap.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include "safemap.hpp"
using namespace std;

#define counts 2000
#define countsTime 50

#define wthreadNum 4            //写线程个数
#define rthreadNum 4            //读线程个数
#define ethreadNum 4            //擦拭线程个数
#define uthreadNum 4            //更新线程个数
#define fthreadNum 4            //查找线程个数

typedef SafeMap<int, int> MYMAP_t;

MYMAP_t _myMap;

void* write(void* arg)
{
    (void *)arg;
    //    while(1)
    for(int j=0; j<countsTime; j++)
    {
        for(int i=0; i<counts; i++)
        {
            _myMap.insert(i, i);
        }
        usleep(1);
    }

    return NULL;
}

void* read(void* arg)
{
    (void *)arg;
    //    while(1)
    for(int j=0; j<countsTime; j++)
    {
        MYMAP_t::iterator _beginItr = _myMap.begin();

        for(; _beginItr!=_myMap.end(); _beginItr++)
        {

        }

        std::cout<<"_myMap.begin value:"<<_myMap.begin().second<<std::endl;
        std::cout<<"size:"<<_myMap.size()<<std::endl;
        usleep(2);

    }

    return NULL;
}

void* erase(void* arg)
{
    (void *)arg;
    //    while(1)
    for(int j=0; j<countsTime; j++)
    {
        for(int i=0; i<counts; i++)
        {
            _myMap.erase(i);
            usleep(2);

        }

    }

    return NULL;
}

void* update(void* arg)
{
    (void *)arg;
    //    while(1)
    for(int j=0; j<countsTime; j++)
    {
        for(int i=0; i<counts; i++)
        {
            _myMap.update(i, 1200);
            usleep(3);
        }

    }

    return NULL;
}

void* find(void* arg)
{
    (void *)arg;
    //    while(1)
    for(int j=0; j<countsTime; j++)
    {
        for(int i=0; i<counts; i++)
        {
            _myMap.find(i);
            usleep(4);
        }
    }

    return NULL;
}


void test_safeMap(){
    pthread_t _theadId[wthreadNum + rthreadNum + ethreadNum + uthreadNum + fthreadNum];

    for(int i=0; i<wthreadNum + rthreadNum + ethreadNum + uthreadNum + fthreadNum; i++)
    {
        if(i < wthreadNum)
        {
            pthread_create(&_theadId[i], NULL, write, NULL);
        }
        else if(i < wthreadNum + rthreadNum)
        {
            pthread_create(&_theadId[i], NULL, read, NULL);
        }
        else if(i < wthreadNum + rthreadNum + ethreadNum)
        {
            pthread_create(&_theadId[i], NULL, erase, NULL);
        }
        else if(i < wthreadNum + rthreadNum + ethreadNum + uthreadNum)
        {
            pthread_create(&_theadId[i], NULL, update, NULL);
        }
        else
        {
            pthread_create(&_theadId[i], NULL, find, NULL);
        }
    }

    pthread_join(_theadId[wthreadNum + rthreadNum + ethreadNum + uthreadNum - 1], NULL);

    std::cout<<_myMap.size()<<std::endl;
}

map<int, int> stdMap;
void test_stdMap(){

    //启动
    pthread_t _theadId[wthreadNum + rthreadNum + ethreadNum + uthreadNum + fthreadNum];

    for(int i=0; i<wthreadNum + rthreadNum + ethreadNum + uthreadNum + fthreadNum; i++)
    {
        if(i < wthreadNum)
        {//write
            pthread_create(&_theadId[i], NULL, [](void *args)->void* {

                //    while(1)
                for(int j=0; j<countsTime; j++)
                {
                    for(int i=0; i<counts; i++)
                    {
                        if(stdMap.find(i)==stdMap.end())
                            stdMap[i]=i;
                        //stdMap.insert(i, i);
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
                    auto _beginItr = stdMap.begin();

                    for(; _beginItr!=stdMap.end(); _beginItr++)
                    {

                    }

                    std::cout<<"_myMap.begin value:"<<stdMap.begin()->second<<std::endl;
                    std::cout<<"size:"<<stdMap.size()<<std::endl;
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

                        stdMap.erase(i);
                        usleep(2);

                    }

                }

                return NULL;
            }, NULL);
        }
        else if(i < wthreadNum + rthreadNum + ethreadNum + uthreadNum)
        {//update
            pthread_create(&_theadId[i], NULL, [](void *args)->void* {

                //    while(1)
                for(int j=0; j<countsTime; j++)
                {
                    for(int i=0; i<counts; i++)
                    {
                        stdMap[i]=1200;
                        usleep(3);
                    }

                }

                return NULL;
            }, NULL);
        }
        else
        {//find
            pthread_create(&_theadId[i], NULL, [](void *args)->void* {

                //    while(1)
                for(int j=0; j<countsTime; j++)
                {
                    for(int i=0; i<counts; i++)
                    {
                        stdMap.find(i);
                        usleep(4);
                    }
                }

                return NULL;
            }, NULL);
        }
    }

    pthread_join(_theadId[wthreadNum + rthreadNum + ethreadNum + uthreadNum - 1], NULL);

    std::cout<<stdMap.size()<<std::endl; }

int main(void)
{
    //写读删更查，每个操作执行2000次，迭代５０次。最后map个数应该是０个
    //test_safeMap();
    test_stdMap();

    //结果：stdMap不是线程安全的，
    return EXIT_SUCCESS;
}
