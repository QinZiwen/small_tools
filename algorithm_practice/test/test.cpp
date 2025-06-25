#include <iostream>
#include <vector>
#include <list>

struct Node {
    int data;
    Node* next;
};

class ListClt {
public:
    void constructList(const std::vector<int>& datas) {
        for (auto d : datas) {
            Node* n = new Node();
            n->data = d;
            n->next = nullptr;
            if (m_head == nullptr) {
                m_head = n;
                m_tail = m_head;
            } else {
                m_tail->next = n;
            }
        }
    }

    void revertList() {
        if (m_head == nullptr) {
            return;
        }

        Node* curr = m_head;
        Node* next = m_head->next;
        while (next) {
            Node* next2 = nullptr;
            if (next->next) {
                next2 = next->next;
            }

            next->next = curr;
            if (curr == m_head) {
                curr->next = nullptr;
            }

            curr = next;
            next = next2;
        }
    }

private:
    Node* m_head = nullptr;
    Node* m_tail = nullptr;
};

void testList() {
    std::list<int> l1;
    std::list<char> l2;
    std::list<double> l3;
    std::cout << "int list max size: " << l1.max_size() << std::endl;
    std::cout << "char list max size: " << l2.max_size() << std::endl;
    std::cout << "double list max size: " << l3.max_size() << std::endl;
}

int main(int argc, char** argv) {
    testList();
    return 0;
}