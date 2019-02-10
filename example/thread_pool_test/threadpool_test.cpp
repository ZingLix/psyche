#include "psyche/ThreadPool.h"
#include <iostream>
using namespace psyche;

void test() {
	std::cout << "asdf" << std::endl;
}

void test1(std::string example) {
	std::cout << example << std::endl;
}

void go() {
	ThreadPool tp;
	tp.Execute(test);
	tp.Execute(test1, "test2");
	std::string t("test3");
	tp.Execute([=]() {test1(t); });
	tp.stop();
}

int main() {
	go();
}