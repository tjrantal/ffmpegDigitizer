/**
	Compile:
	g++ threadTest.cpp -o threadTest -lpthread -std=gnu++11
*/

#include <thread>			//For threading, tracking opens up a new thread
#include <stdio.h>			//For output
#include <unistd.h>			//For sleep
#include <vector>

class ThreadClass{
	private:
		int concurrentThreads;
	public:
		int threadNo;
		std::vector<std::thread> testThread;
		ThreadClass(int concurrentThreadsIn);
		~ThreadClass();
		void runTest(int threadNo);
		void runTest2();
		void runClass();	/*Use the threadNo private variable from the object*/
		void runClass2();	/*Use the threadNo private variable from the object*/
};

int main(int argc, char** args){
	
	ThreadClass* tc = new ThreadClass(5);
	//tc->runClass();
	tc->runClass2();
	printf("\nGot to running\n");
	/*
	while (tc->threadNo < 4){
		sleep(1);
	}
	*/
	
	for (int i = 0; i<tc->testThread.size();++i){
		tc->testThread[i].join();
	}
	
	printf("\nAll done\n");
	return 0;
}

ThreadClass::ThreadClass(int concurrentThreadsIn){
	concurrentThreads = concurrentThreadsIn;
	
}

ThreadClass::~ThreadClass(){
	testThread.clear();
}

void ThreadClass::runClass(){
	testThread = std::vector<std::thread>();
	//std::thread testThread[concurrentThreads];
	threadNo =0;
	for (int i = 0; i<concurrentThreads;++i){
		//testThread[i] = std::thread(&ThreadClass::runTest,this,i);
		testThread.push_back(std::thread(&ThreadClass::runTest,this,i));
	}

}

void ThreadClass::runClass2(){
	testThread = std::vector<std::thread>();
	//std::thread testThread[concurrentThreads];
	threadNo =0;
	for (int i = 0; i<concurrentThreads;++i){
		++threadNo;
		testThread.push_back(std::thread(&ThreadClass::runTest2,this));
	}

}

/*This will not work, cannot use a method from a class*/
void ThreadClass::runTest(int tNo){
	for (int i = 0; i<tNo;++i){
		printf("T %d l %d\n",tNo,i);
		fflush(stdout);
		sleep(1);
	}
	++threadNo;
}

void ThreadClass::runTest2(){
	for (int i = 0; i<threadNo;++i){
		printf("T %d l %d\n",threadNo,i);
		fflush(stdout);
		sleep(1);
	}
}
