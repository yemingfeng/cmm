### 使用 c++ 实现的简单内存池

- [x] 简单的内存分配和释放
- [ ] 增加内存块申请的最优匹配算法
- [ ] 多线程支持
- [ ] ...

#### 运行
```shell script
mkdir build
cd build
cmake -j 6 ..
make
./cmm
```

#### 原理
##### Node 节点(用于存储真实的内存空间)
```c++
    class Node {
    public:
        // 真实开辟的空间
        char *data;
        // 开辟空间的大小
        size_t size;
        // 指向下一块的 node
        Node *next;

        Node() {
            data = nullptr;
            size = 0;
            next = nullptr;
        }

        Node(size_t size) {
            this->size = size;
            this->data = new char[size];
            next = nullptr;
        }

        ~Node() {
            delete data;
        }
    };
```

##### Pool 逻辑
pool 维护一个 Node* free，用于表示当前已经申请并且可用的 Node 列表，注意这是一个**哨兵 node**

###### 申请内存
- 当申请一块内存空间，首先从 free 列表查找，如果存在 node 满足 size_t >= size，则使用该 node
- 如果不存在满足条件的 node，则开辟一个新的空间，核心逻辑如下 
```c++
        Node *findCanUse(size_t size) {
            Node *result = nullptr;
            // 查找能够满足此次开辟空间的 node
            // 注意：这里是判断 it->next 是否满足条件，这样的好处是易于根据 it 节点移除 it->next 节点
            Node *it = free;
            while (it) {
                if (it->next && it->next->size >= size) {
                    // 将这个节点移除
                    result = it->next;
                    it->next = it->next->next;
                    break;
                }
                it = it->next;
            }
            return result;
        }

        // 申请一块内存空间
        Node *allocate(size_t size) {
            // 查找 free 列表可用的 node
            Node *result = findCanUse(size);

            // 说明 free 列表未找到可用的 node，需要重新开辟这块空间
            if (!result) {
                std::cout << "未找到合适的空间，需要重新开辟：" << size << std::endl;
                result = new Node(size);
            }

            return result;
        }
```

###### 释放内存
直接将 node 加入到 free 列表中
```c++
        // 释放一块内存空间
        void deallocate(Node *node) {
            // 简单的时间：将 node 加入到 free 列表最后
            Node *it = free;
            while (it->next) {
                it = it->next;
            }
            it->next = node;
        }
```

#### 使用
```c++
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
```