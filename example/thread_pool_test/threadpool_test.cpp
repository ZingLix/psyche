#include "psyche/thread_pool.h"
#include <iostream>
using namespace psyche;

void test() {
    std::cout << "asdf" << std::endl;
}

void test1(std::string example) {
    std::cout << example << std::endl;
}

int get() {
    return 5;
}

void go() {
    ThreadPool tp;
    tp.execute(test);
    tp.execute(test1, "test2");
    std::string t("test3");
    tp.execute([=]() { test1(t); });
    auto res = tp.execute(get);
    std::cout << res.get() << std::endl;
    std::cin.get();
    tp.stop();
}

int main() {
    go();
}
