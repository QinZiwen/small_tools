#include <iostream>
#include <vector>
#include <unordered_map>

struct Node {
    int value = 0;
    Node* left = nullptr;
    Node* right = nullptr;

    Node(int v) : value(v) {}
    ~Node() {
        std::cout << "destory Node, value: " << value << std::endl;
        delete left;
        delete right;
        left = nullptr;
        right = nullptr;
    }
};

Node* dfs(const std::vector<int>& preorder, const std::unordered_map<int, int>& inorder_map, int i, int l, int r) {
    if (l > r) return nullptr;

    Node* root = new Node(preorder[i]);
    int m = inorder_map.at(preorder[i]);
    std::cout << "value: " << root->value << ", m: " << m
              << ", i: " << i << ", l: " << l << ", r: " << r << std::endl;

    root->left = dfs(preorder, inorder_map, i + 1, l, m - 1);
    root->right = dfs(preorder, inorder_map, i + 1 + m - l, m + 1, r);
    return root;
}

Node* build_tree(const std::vector<int>& preorder, const std::vector<int>& inorder) {
    std::unordered_map<int, int> inorder_map;
    for (size_t i = 0; i < inorder.size(); ++i) {
        inorder_map.emplace(inorder[i], i);
    }

    return dfs(preorder, inorder_map, 0, 0, inorder.size() - 1);
}

void print_tree_preorder(Node* node) {
    if (!node) return;
    std::cout << " " << node->value;
    print_tree_preorder(node->left);
    print_tree_preorder(node->right);
}

int main() {
    std::vector<int> preorder = {3, 9, 2, 1, 7};
    std::vector<int> inorder = {9, 3, 1, 2, 7};

    Node* root = build_tree(preorder, inorder);
    print_tree_preorder(root);
    std::cout << std::endl;
    delete root;
    return 0;
}
