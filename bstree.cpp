/*
2022.4.29(am) complete bs tree
2022.4.29(pm) try to improve to avl tree
2022.4.30   add single rotate and double rotate


编译
clang bstree.cpp -o bstree.out -std=c++2a -lstdc++
*/

#include<iostream>
#include<cassert>
#include<vector>
#include<type_traits>
#include<algorithm>
#include<stack>
#include<queue>
using namespace std;

    enum class rotate_type
    {
        balance,
        left_rotate, 
        right_rotate,
        left_right_rotate,
        right_left_rotate,
    };

template <typename T = int>
#ifndef _WIN32   //vs2019编译不过去
requires (std::is_same<T, int>::value || std::is_same<T, char>::value)
#endif
class bstree
{
public:
    struct tree_node
    {
        T val;
        tree_node* left = nullptr;
        tree_node* right = nullptr;
        tree_node(T val) :val(val) {}
        tree_node() {}
    };
    using node = tree_node;
    using pnode = node*;//vector<typename bstree<T>::pnode>
    using cpnode = node const*;

    bstree() {}
    pnode get_root() { return root; }
    bool insert(T val);


    struct find_struc
    {
        pnode father;
        pnode current;
    };
    auto find(T val);
    void pre_order(pnode root);
    void in_order(pnode root);
    void level_order(pnode root);
    void post_order(pnode root);
    int child_counts(pnode node);
    bool delete_node(T val);
    int get_node_depth(const cpnode node);
    int get_node_height(const pnode node);
    rotate_type is_node_balanced(const pnode node);
    void rotate(pnode node, rotate_type type);
private:
    pnode root = nullptr; //vc会默认初始化为空指针,gcc不会,所以指定初始化为null比较稳妥

};

//
//true插入成功,false失败(出现重复数的插入)
//
template<typename T>
bool bstree<T>::insert(T val)
{
    //先查找一次,出现相同的插入失败
    auto [a, b] = find(val);
    if (a || b)
        return false;

    pnode t = nullptr;

    if (!root)
    {
        root = new node; //除了这里,其他地方都不应该修改root
        root->val = val;
    }
    else {
        t = root;
        while (t) {
            if (val > t->val) //插入的数比根节点大
            {
                auto rnode = t->right;
                if (rnode)
                    t = rnode;
                else //右节点为空,直接插入就可以
                {
                    t->right = new node(val);
                    break;
                }
            }
            else if (val < t->val) {
                auto lnode = t->left;
                if (lnode)
                    t = lnode;
                else
                {
                    t->left = new node(val);
                    break;
                }
            }
        }

        auto type = rotate_type::balance;
        auto father = find(t->val).father;
        if ((type = is_node_balanced(father)) != rotate_type::balance) {
            rotate(father, type);
        }


    }



    return true;
}

template<typename T>
void bstree<T>::pre_order(pnode root) {
    if (!root)
        return;
    else {
        cout << root->val << " ";
        pre_order(root->left);
        pre_order(root->right);
    }
}

template<typename T>
void bstree<T>::in_order(pnode root) {
    if (!root)
        return;
    else {
        in_order(root->left);
        cout << root->val << " ";
        in_order(root->right);
    }
}

template<typename T>
void bstree<T>::post_order(pnode root) {
    if (!root)
        return;
    else {
        post_order(root->left);
        post_order(root->right);
        cout << root->val << " ";
    }
}

template<typename T>
void bstree<T>::level_order(pnode root) {
    pnode t = nullptr;
    queue<pnode> que;
    que.push(root);

    while(que.size()){
        t = que.front();
        que.pop();
        if(!t) continue;
        //
        cout << t->val << endl;
        //
        que.push(t->left);
        que.push(t->right);

    }
    

}

template<typename T>
auto bstree<T>::find(T val) {
    if (!root)
        return find_struc{0, 0};

    auto t = root;
    pnode father = nullptr;
    while (t) {
        if (val > t->val) //要查找的值大于这个节点,应该找右节点
        {
            if (t->right) {
                father = t;
                t = t->right;
            }
            else
                return find_struc{0, 0};
        }
        else if (val < t->val) {
            if (t->left) {
                father = t;
                t = t->left;
            }
            else
                return find_struc{0, 0};

        }
        else //相等
        {
            return find_struc{0, 0};
        }
    }

    return find_struc{0, 0};
}

//
//给定一个树节点,返回这个树节点的树叶数量(仅左右子树),所以最大为2
//
template<typename T>
int bstree<T>::child_counts(pnode node)
{
    if (node->left && node->right) //有左右节点
        return 2;
    else if (node->left && !node->right) //只有左节点
        return 1;
    else if (!node->left && node->right) //只有右节点
        return 1;
    else //没有节点
        return 0;
}

template<typename T>
bool bstree<T>::delete_node(T val) {
    auto [father, current] = find(val);
    if (!father && !current)
        return false;

    if (child_counts(current) == 0) //没有子节点,直接删除即可
    {
        if (father == nullptr)
        {
            root = nullptr;
            return true;
        }

        if (father->left && father->left->val == current->val)
            father->left = nullptr;
        else if (father->right && father->right->val == current->val)
            father->right = nullptr;
        else
            assert(0); //find方法有误
    }
    else if (child_counts(current) == 1) {//要删除的节点只有一个子节点

        if (father == 0) {
            if (father->left)
                root = father->left;
            else
                root = father->right;

            return true;
        }

        if (father->left && father->left->val == current->val) { //要删除的节点是父节点的左节点
            if (current->left) //
                father->left = current->left;
            else
                father->left = current->right;
        }
        else { //删除的节点是父节点的右节点
            assert(father->right);

            if (current->left) //
                father->right = current->left;
            else
                father->right = current->right;
        }
    }
    else if (child_counts(current) == 2) {//要删除的节点有两个子节点
        //将左子树挂到右子树的最左子树

        if (father == nullptr)
            root = current->right;//根节点换成原右节点
        else {
            if (father->left && father->left->val == current->val)
                father->left = current->right;
            else
                father->right = current->right;
        }


        auto recur_left_end = current->right;
        while (true) {
            if (recur_left_end->left) //有左子树，不断地向下找最左子树
                recur_left_end = recur_left_end->left;
            else
                break;
        }
        recur_left_end->left = current->left;


    }
    else
        assert(0); //child_counts方法有误

    return true;
}

template<typename T>
int bstree<T>::get_node_depth(const cpnode node)
{
    //因为每个节点只能有一个父节点,不断的向上找父节点,就可以求出节点的深度
    int depth = 1;
    auto father = find(node->val).father; //auto [father,current]
    while (father) {
        father = (find(father->val)).father;
        depth++;
    }


    return depth;
}

template<typename T>
int bstree<T>::get_node_height(const pnode node)
{
    if (!node)
        return 0;

    if (!node->left && !node->right)
        return 1;

    int h1 = get_node_height(node->left) + 1;
    int h2 = get_node_height(node->right) + 1;

    return std::max(h1, h2);
}

//确定某个节点是否平衡
template<typename T>
rotate_type bstree<T>::is_node_balanced(const pnode node)
{
    if (!node)
        return rotate_type::balance;  //true还是false?

    int left_child_height = get_node_height(node->left);
    int right_child_height = get_node_height(node->right);
    //debug   
    cout << "root val : " << node->val << endl;
    cout << "left child height : " << left_child_height << endl;
    cout << "right child height : " << right_child_height << endl;
    //

        //左子树过高
    if (left_child_height - right_child_height > 1)
    {
        if (node->left->right)
            return rotate_type::left_right_rotate;
        return rotate_type::right_rotate;
    }
    else if (right_child_height - left_child_height > 1) {
        if (node->right->left)
            return rotate_type::right_left_rotate;
        return rotate_type::left_rotate;
    }



    return rotate_type::balance;
}

template<typename T>
void bstree<T>::rotate(pnode node, rotate_type type)
{
    pnode child = nullptr; //child是子树新的根

    //左旋
    if (type == rotate_type::left_rotate) {
        child = node->right;
        node->right = child->left;
        child->left = node;
    }
    else if (type == rotate_type::right_rotate) {
        child = node->left;
        node->left = child->right;
        child->right = node;
    }
    else if (type == rotate_type::left_right_rotate) {
        child = node->left;
        node->left = child->right;
        node->left->left = child;

        child = node->left;
        child->right = 0;
        child->left->right = 0;
        node->left = child->right;
        child->right = node;
    }
    else if(type == rotate_type::right_left_rotate)
    {
        child = node->right;
        node->right = child->left;
        node->right->right = child;

        child = node->right;
        node->right = child->left;
        child->left = node;
        
        child->right->left = 0; //原来节点是有值的，转之后置空

    }

    //因为旋转子树,需要更换根节点,所以要让父节点有正确的值
    auto father = find(node->val).father;
    if (!father)
        root = child;
    else
    {//判断原根是上级父节点的左子树还是右子树
        if (father->left->val == node->val)
            father->left = child;
        else
            father->right = child;
    }
}

int main()
{
    bstree<int> test;
    test.insert(25);
    test.insert(18);
    test.insert(69);
    test.insert(5);
    test.insert(20);
    test.insert(32);
    test.insert(31);
    std::cout << "ok\n";
    return 0;
}
