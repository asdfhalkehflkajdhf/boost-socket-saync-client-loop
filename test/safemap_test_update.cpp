
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


int main(void)
{
    //写读删更查，每个操作执行2000次，迭代５０次。最后map个数应该是０个
    //test_safeMap();
    _myMap.insert(1, 2);
	_myMap.insert(2, 3);
	
	cout<<_myMap[2]<<endl;
	_myMap[2]=4;
	cout<<_myMap[2]<<endl;

	if(_myMap.find(3)!=_myMap.end())
		cout<<_myMap[3]<<endl;
	
	if(_myMap.find(2)!=_myMap.end())
		cout<<_myMap[2]<<endl;

    //结果：stdMap不是线程安全的，
    return EXIT_SUCCESS;
}
