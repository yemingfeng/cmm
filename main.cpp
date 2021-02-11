#include <iostream>
#include <string>
#include <ostream>
#include <ctime>

#include "./cmm/cmmpool.h"

class Person {
public:
    int id;
    int sex;

    friend std::ostream &operator<<(std::ostream &out, Person &p) {
        out << "id = " << p.id << "\tsex = " << p.sex << "\tpointer = " << (&p) << std::endl;
        return out;
    }
};

void mock(Person *person) {
    srand((unsigned) time(nullptr));
    int id = std::abs(std::rand() % 100);
    person->id = id;
    person->sex = id;
}

int main() {
    CMM::Pool pool;

    size_t size = sizeof(Person);

    auto oneNode = pool.allocate(size);
    auto *one = reinterpret_cast<Person *>(oneNode->data);
    mock(one);
    std::cout << *one;
    // 测试释放内存逻辑
    pool.deallocate(oneNode);

    auto twoNode = pool.allocate(size);
    auto *two = reinterpret_cast<Person *>(twoNode->data);
    mock(two);
    // 这里输出的内存地址和 oneNode 是一样的
    std::cout << *two;

    auto threeNode = pool.allocate(size);
    auto *three = reinterpret_cast<Person *>(threeNode->data);
    mock(three);
    // 这里输出的内存地址和 oneNode 是不一样的
    std::cout << *three;

    return 0;
}