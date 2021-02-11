//
// Created by yemingfeng on 2021/2/10.
//

#ifndef CMM_CMMPOOL_H
#define CMM_CMMPOOL_H

namespace CMM {

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

    class Pool {
    private:
        Node *free;

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

    public:
        Pool() {
            free = new Node();
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

        // 释放一块内存空间
        void deallocate(Node *node) {
            // 简单的时间：将 node 加入到 free 列表最后
            Node *it = free;
            while (it->next) {
                it = it->next;
            }
            it->next = node;
        }

        ~Pool() {
            Node *it = free;
            while (it) {
                Node *next = it->next;
                delete it;
                it = next;
            }
        }
    };

}

#endif //CMM_CMMPOOL_H
