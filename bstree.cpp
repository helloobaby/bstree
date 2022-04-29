#include<iostream>
#include<cassert>
#include<tuple>
#include<type_traits>
using namespace std;

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
    using pnode = node*;

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
    void post_order(pnode root);

    int child_counts(pnode node);

    bool delete_node(T val);
private:
    pnode root = nullptr; //vc会默认初始化为空指针,gcc不会

};

//
//true插入成功,false失败(出现重复数的插入)
//
template<typename T>
bool bstree<T>::insert(T val)
{
    //先查找一次,出现相同的插入失败
    auto [a,b] = find(val);
    if(a || b)
        return false;

    if (!root)
    {
        root = new node; //除了这里,其他地方都不应该修改root
        root->val = val;
    }
    else {
        pnode t = root;
        while (t) {
            if (val > t->val) //插入的数比根节点大
            {
                auto rnode = t->right;
                if (rnode)
                    t = rnode;
                else //右节点为空,直接插入就可以
                {
                    t->right = new node(val);
                    return true;
                }
            }
            else if (val < t->val) {
                auto lnode = t->left;
                if (lnode)
                    t = lnode;
                else
                {
                    t->left = new node(val);
                    return true;
                }
            }
        }
    }

    //assert(0);
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
void bstree<T>::in_order(pnode root){
    if (!root)
        return;
    else {
        in_order(root->left);
        cout << root->val << " ";
        in_order(root->right);
    }
}

template<typename T>
void bstree<T>::post_order(pnode root){
    if(!root)
        return;
    else{
        post_order(root->left);
        post_order(root->right);
        cout << root->val << " ";
    }
}

template<typename T>
auto bstree<T>::find(T val){
    if(!root) 
        return find_struc(0,0);
    
    auto t = root;
    auto father = root;
    while(t){
        if(val > t->val) //要查找的值大于这个节点,应该找右节点
        {
            if(t->right){
                father = t;
                t = t->right;
            }
            else
                return find_struc(0,0);
        }
        else if(val < t->val){
            if(t->left){
                father = t;
                t=t->left;
            }else
                return find_struc(0,0);

        }
        else //相等
        {   
            return find_struc(father,t);
        }
    }

    assert(0);
}

//
//给定一个树节点,返回这个树节点的树叶数量
//
template<typename T>
int bstree<T>::child_counts(pnode node)
{
    if(node->left && node->right) //有左右节点
        return 2;
    else if( node->left && !node->right) //只有左节点
        return 1;
    else if( !node->left && node->right) //只有右节点
        return 1;
    else //没有节点
        return 0;
}

template<typename T>
bool bstree<T>::delete_node(T val){
    auto [father,current] = find(val);
    if(!father && !current)
        return false;

    if(child_counts(current) == 0) //没有子节点,直接删除即可
    {
        if(father->left && father->left->val == current->val)
            father->left = nullptr;
        else if(father->right && father->right->val == current->val)
            father->right = nullptr;
        else
            assert(0); //find方法有误
    }
    else if(child_counts(current) == 1){//要删除的节点只有一个子节点
        if(father->left && father->left->val == current->val){ //要删除的节点是父节点的左节点
            if(current->left) //
                father->left = current->left;
            else
                father->left = current->right;
        }else{ //删除的节点是父节点的右节点
            assert(father->right);

            if(current->left) //
                father->right = current->left;
            else
                father->right = current->right;
        }
    } 
    else if(child_counts(current) == 2){//要删除的节点有两个子节点
        //将左子树挂到右子树的最左子树


        father->right = current->right;

        auto recur_left_end = current->right;
        while(true){
            if(recur_left_end->left) //有左子树，不断地向下找最左子树
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


int main()
{
    bstree<int> test;
    test.insert(1);
    test.insert(3);
    test.insert(2);
    test.insert(5);
    test.insert(7);
    test.insert(9);
    test.insert(8);
    test.insert(4);
    test.insert(6);

#if 0 //测试重复插入
    auto insert = test.insert(100);
    cout.setf(ios_base::boolalpha);
    cout << "insert : " << insert << endl;
#endif

#if 0 //测试树的形状是否正确
    test.pre_order(test.get_root());
    cout << endl;
    test.in_order(test.get_root());//升序序列
    cout << endl;
    test.post_order(test.get_root()); 
    cout << endl;
#endif

#if 0 //测试find和child_counts
    auto [father,current] = test.find(6);
    if(!father && !current)
        cout << "val not found\n";
    else{
        cout << "father -> val : " << father->val << " ";
        cout << "find val -> : " << current->val << endl;
    }
    cout << "child counts :" << test.child_counts(father) << endl;
#endif

#if 0 //测试没有子树节点删除
    bool delete_ok = test.delete_node(8);
    cout << "delete ok : " << delete_ok << endl;
    test.in_order(test.get_root());
    cout << endl;
#endif 

#if 0//测试单节点子树删除
    bool delete_ok = test.delete_node(9);
    cout << "delete ok : " << delete_ok << endl;
    test.in_order(test.get_root());
    cout << endl;
#endif

#if 1 //测试双节点子树删除
    bool delete_ok = test.delete_node(7);
    cout << "delete ok : " << delete_ok << endl;
    test.in_order(test.get_root());
    cout << endl;
#endif



    return 0;
}