//#include<iostream>
#include<queue>
#include<stack>
#include<vector>
#include<string>
using namespace std;

template <typename T>
class binTreeNode {
    T data;
    binTreeNode* left;
    binTreeNode* right;
public:

    template <typename U>
    friend class huffmanTree;
    template <typename U>
    friend bool search(string& s, binTreeNode<U>* root, U item);

    binTreeNode(T data) {
        this->data = data;
        this->left = NULL;
        this->right = NULL;
    }
    binTreeNode(T data, binTreeNode* left, binTreeNode* right) :data(data), left(left), right(right) {}

    binTreeNode(const binTreeNode<T>& node) :data(node.data), left(node.left), right(node.right) {
        //this=node;
    }

    bool operator>(const binTreeNode<T>& a)const {
        if (this->data > a.data) return true;
        else return false;
    }//重载操作符，使其能被优先队列的less函数识别
    bool operator<(const binTreeNode<T>& a)const {
        if (this->data < a.data) return true;
        else return false;
    }//重载操作符，使其能被优先队列的greater函数识别
};

template <typename T>
class huffmanTree {
    binTreeNode<T>* root;
    void deleteTree(binTreeNode<T>* root);
public:
    huffmanTree() { root = NULL; }
    huffmanTree(vector<T> weight);
    virtual ~huffmanTree() {
        deleteTree(this->root);
    }
    string encode(T item);
};

template <typename T>
void huffmanTree<T>::deleteTree(binTreeNode<T>* root) {//递归实现
    if (root->left) deleteTree(root->left);
    if (root->right) deleteTree(root->right);
    delete root;
    return;
}

template <typename T>
huffmanTree<T>::huffmanTree(vector<T> weight) {
    priority_queue<binTreeNode<T>, vector<binTreeNode<T>>, greater<binTreeNode<T>>> q;
    for (int i = 0; i < weight.size(); i++) {
        binTreeNode<T> temp(weight[i]);
        q.push(temp);
    }
    binTreeNode<T>* parent, * lchild, * rchild;
    while (q.size() > 1) {
        lchild = new binTreeNode<T>(q.top());
        q.pop();
        rchild = new binTreeNode<T>(q.top());
        q.pop();
        parent = new binTreeNode<T>(lchild->data + rchild->data);
        parent->left = lchild;
        parent->right = rchild;

        q.push(*parent);
    }
    this->root = new binTreeNode<T>(q.top());
}

template <typename T>
string huffmanTree<T>::encode(T item) {//加密：返回传入的元素对应的编码字符串
    string s;
    search(s, this->root, item);
    return s;
}

template <typename U>
bool search(string& s, binTreeNode<U>* root, U item) {
    bool s1, s2;
    if (root == NULL) return false;
    s1 = search(s, root->left, item);
    s2 = search(s, root->right, item);
    if (s1) { s.insert(0, "0"); return true; }
    if (s2) { s.insert(0, "1"); return true; }
    if (root->data == item && !root->left && !root->right) return true;//确保这个节点是叶！！！
    return false;
}

/*
int main(){
    vector<int> w{6,2,3,3,4};
    huffmanTree<int> tree(w);
    cout<<tree.encode(3);
}
*/
