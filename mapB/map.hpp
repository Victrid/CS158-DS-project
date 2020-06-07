/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include "exceptions.hpp"
#include "utility.hpp"
#include <cstddef>
#include <functional>
#include <iostream>

#include <cmath>
using std::max;

namespace sjtu {

template <
    class Key,
    class T,
    class Compare = std::less<Key>>
class map {
public:
    typedef pair<const Key, T> value_type;
    struct node {
        value_type* data;
        int height, size;
        node *left, *right, *prev, *next;

        node(const value_type* d = nullptr, int height = 1, int size = 1, node* left = nullptr, node* right = nullptr, node* prev = nullptr, node* next = nullptr) : data(nullptr), height(height), size(size), left(left), right(right), prev(prev), next(next) {
            if (d != nullptr)
                data = new value_type(*d);
        }
        ~node() {
            if (data != nullptr)
                delete data;
        }
    };
    template <typename C>
    void swap(C& c1, C& c2) {
        C tmp = c1;
        c1    = c2;
        c2    = tmp;
        return;
    }

private:
    //AVL part
    node* __root;
    node* __begin;
    node* __end;

    inline int __get_factor(node*& root) const {
        if (root == nullptr)
            return 0;
        return (root->left == nullptr ? 0 : root->left->height) - (root->right == nullptr ? 0 : root->right->height);
    }
    inline void __set_root_height(node* root) {
        if (root == nullptr)
            return;
        root->height = max((root->left == nullptr ? 0 : root->left->height), (root->right == nullptr ? 0 : root->right->height)) + 1;
    }
    inline void __set_root_size(node* root) {
        if (root == nullptr)
            return;
        root->size = (root->left == nullptr ? 0 : root->left->size) + (root->right == nullptr ? 0 : root->right->size) + 1;
    }
    //AVL rotate
    node* __right_rotate(node*& root) {
        node* nl   = root->left;
        root->left = nl->right;
        nl->right  = root;
        __set_root_height(root);
        __set_root_size(root);
        __set_root_height(nl);
        __set_root_size(nl);
        root = nl;
        return root;
    }
    node* __left_rotate(node*& root) {
        node* nr    = root->right;
        root->right = nr->left;
        nr->left    = root;
        __set_root_height(root);
        __set_root_size(root);
        __set_root_height(nr);
        __set_root_size(nr);
        root = nr;
        return root;
    }
    //Supplimentary
    inline void __left_right(node*& t) {
        __left_rotate(t->left);
        __right_rotate(t);
    }
    inline void __right_left(node*& t) {
        __right_rotate(t->right);
        __left_rotate(t);
    }
    //find the specific storage node with Key to_query
    node* __query_trav_(const Key& to_query, node* root) const {
        if (root == nullptr)
            return nullptr;
        //*** that "fuck you" hit me hard
        if (Compare()(root->data->first, to_query))
            return __query_trav_(to_query, root->right);
        else if (Compare()(to_query, root->data->first))
            return __query_trav_(to_query, root->left);
        else
            return root;
    }
    node* __add_entry(const value_type& value, node*& t) {
        node* tmp;
        if (t == nullptr) {
            t = new node(&value);
            return t;
        } else {
            if (Compare()(value.first, t->data->first)) {
                if (t->left == nullptr) {
                    tmp       = __add_entry(value, t->left);
                    tmp->next = t;
                    tmp->prev = t->prev;
                    t->prev   = tmp;
                    if (tmp->prev == nullptr)
                        __begin = tmp;
                    else
                        tmp->prev->next = tmp;
                } else {
                    tmp = __add_entry(value, t->left);
                }
                __set_root_height(t);
                __set_root_size(t);
                if (__get_factor(t) == 2) {
                    if (Compare()(value.first, t->left->data->first))
                        __right_rotate(t);
                    else
                        __left_right(t);
                }
                return tmp;
            } else {
                if (t->right == nullptr) {
                    tmp             = __add_entry(value, t->right);
                    tmp->prev       = t;
                    tmp->next       = t->next;
                    t->next         = tmp;
                    tmp->next->prev = tmp;
                } else {
                    tmp = __add_entry(value, t->right);
                }
                __set_root_height(t);
                __set_root_size(t);
                if (__get_factor(t) == -2) {
                    if (Compare()(value.first, t->right->data->first)) {
                        __right_left(t);
                    } else {
                        __left_rotate(t);
                    }
                }
                return tmp;
            }
        }
    }
    void __delete_entry(node* obj, node*& t) {
        if (t == nullptr)
            return;
        if (Compare()(obj->data->first, t->data->first)) {
            __delete_entry(obj, t->left);
            if (__get_factor(t) == -2) {
                if (t->right->left != nullptr && __get_factor(t) > 0) {
                    __right_left(t);
                } else {
                    __left_rotate(t);
                }
            }
        } else {
            if (Compare()(t->data->first, obj->data->first)) {
                __delete_entry(obj, t->right);
                if (__get_factor(t) == 2) {
                    if (t->left->right != nullptr && __get_factor(t) < 0) {
                        __left_right(t);
                    } else {
                        __right_rotate(t);
                    }
                }
            } else {
                if (t->left != nullptr && t->right != nullptr) {
                    //swap t with its next node for delete
                    node* nextnode = t->next;
                    swap(t->height, nextnode->height);
                    swap(t->size, nextnode->size);
                    node* tmp;
                    if (nextnode == t->right) {
                        t->right        = nextnode->right;
                        nextnode->left  = t->left;
                        nextnode->right = t;
                        t->left         = nullptr;
                    } else {
                        tmp = t->right;
                        while (tmp->left != nextnode)
                            tmp = tmp->left;
                        tmp->left = t;
                        swap(t->right, nextnode->right);
                        nextnode->left = t->left;
                        t->left        = nullptr;
                    }
                    swap(t, nextnode);
                    __delete_entry(nextnode, t->right);
                    __set_root_height(t);
                    __set_root_size(t);
                    if (__get_factor(t) == 2) {
                        if (t->left->right != nullptr && __get_factor(t) < 0)
                            __left_right(t);
                        else
                            __right_rotate(t);
                    }
                } else {
                    node* todel = t;
                    if (t == __begin) {
                        __begin       = __begin->next;
                        __begin->prev = nullptr;
                    } else {
                        t->prev->next = t->next;
                        t->next->prev = t->prev;
                    }
                    if (t->left == nullptr && t->right == nullptr) {
                        delete todel;
                        t = nullptr;
                        return;
                    }
                    t = t->left != nullptr ? t->left : t->right;
                    delete todel;
                }
            }
        }
        __set_root_height(t);
        __set_root_size(t);
    }

    inline bool __isleaf(node* t) { return t != nullptr && t->size == 1; }

    //traverse when copying
    void ___ctor_inorder_traverse(node* mathis, node* other) {
        if (other->left != nullptr) {
            mathis->left = new node(other->left->data, other->left->height, other->left->size);
            ___ctor_inorder_traverse(mathis->left, other->left);
        }
        if (other->right != nullptr) {
            mathis->right = new node(other->right->data, other->right->height, other->right->size);
            ___ctor_inorder_traverse(mathis->right, other->right);
        }
    }
    void ___ctor_connect_all(node* nothis, node*& npass) {
        if (nothis->left != nullptr)
            ___ctor_connect_all(nothis->left, npass);
        if (npass == nullptr)
            __begin = nothis;
        nothis->prev = npass;
        if (npass != nullptr)
            npass->next = nothis;
        npass = nothis;
        if (nothis->right != nullptr)
            ___ctor_connect_all(nothis->right, npass);
    }
    //traverse to clear
    int ___dtor_postorder_traverse(node* root) {
        if (root == nullptr)
            return 0;
        ___dtor_postorder_traverse(root->left);
        ___dtor_postorder_traverse(root->right);
        delete root;
        return 0;
    }

public:
    class const_iterator;
    class iterator {
        //iterator rewritten. We have prev/next now.
        friend class map;
        friend class const_iterator;

    private:
        node* cur_node;
        const map* mathis;

    public:
        iterator(node* po = nullptr, map* m = nullptr) : cur_node(po), mathis(m) {}
        iterator(const iterator& other) : cur_node(other.cur_node), mathis(other.mathis) {}
        iterator(const const_iterator& other) : cur_node(other.cur_node), mathis(other.mathis) {}
        iterator(const iterator&& other) : cur_node(other.cur_node), mathis(other.mathis) {}
        iterator(const const_iterator&& other) : cur_node(other.cur_node), mathis(other.mathis) {}

        iterator& operator=(const iterator& other) {
            if (this == &other)
                return *this;
            cur_node = other.cur_node;
            mathis   = other.mathis;
            return *this;
        }
        iterator operator++(int) {
            iterator it(*this);
            ++(*this);
            return it;
        }
        iterator& operator++() {
            if (cur_node == nullptr || cur_node->next == nullptr)
                throw invalid_iterator();
            cur_node = cur_node->next;
            return *this;
        }
        iterator operator--(int) {
            iterator it(*this);
            --(*this);
            return it;
        }
        iterator& operator--() {
            if (cur_node == nullptr || cur_node->prev == nullptr)
                throw invalid_iterator();
            cur_node = cur_node->prev;
            return *this;
        }
        value_type& operator*() const { return *cur_node->data; }
        bool operator==(const iterator& rhs) const { return mathis == rhs.mathis && cur_node == rhs.cur_node; }
        bool operator==(const const_iterator& rhs) const { return mathis == rhs.mathis && cur_node == rhs.cur_node; }
        bool operator!=(const iterator& rhs) const { return mathis != rhs.mathis || cur_node != rhs.cur_node; }
        bool operator!=(const const_iterator& rhs) const { return mathis != rhs.mathis || cur_node != rhs.cur_node; }
        value_type* operator->() const noexcept { return cur_node->data; }
    };
    class const_iterator {
        friend map;
        friend class iterator;

    private:
        node* cur_node;
        const map* mathis;

    public:
        const_iterator(node* po = nullptr, const map* m = nullptr) : cur_node(po), mathis(m) {}
        const_iterator(const iterator& other) : cur_node(other.cur_node), mathis(other.mathis) {}
        const_iterator(const const_iterator& other) : cur_node(other.cur_node), mathis(other.mathis) {}
        const_iterator(const iterator&& other) : cur_node(other.cur_node), mathis(other.mathis) {}
        const_iterator(const const_iterator&& other) : cur_node(other.cur_node), mathis(other.mathis) {}

        const_iterator& operator=(const const_iterator& other) {
            if (this == &other)
                return *this;
            cur_node = other.cur_node;
            mathis   = other.mathis;
            return *this;
        }
        const_iterator operator++(int) {
            iterator it(*this);
            ++(*this);
            return it;
        }
        const_iterator& operator++() {
            if (cur_node == nullptr || cur_node->next == nullptr)
                throw invalid_iterator();
            cur_node = cur_node->next;
            return *this;
        }
        const_iterator operator--(int) {
            iterator it(*this);
            --(*this);
            return it;
        }
        const_iterator& operator--() {
            if (cur_node == nullptr || cur_node->prev == nullptr)
                throw invalid_iterator();
            cur_node = cur_node->prev;
            return *this;
        }
        const value_type& operator*() const { return *cur_node->data; }
        bool operator==(const iterator& rhs) const { return mathis == rhs.mathis && cur_node == rhs.cur_node; }
        bool operator==(const const_iterator& rhs) const { return mathis == rhs.mathis && cur_node == rhs.cur_node; }
        bool operator!=(const iterator& rhs) const { return mathis != rhs.mathis || cur_node != rhs.cur_node; }
        bool operator!=(const const_iterator& rhs) const { return mathis != rhs.mathis || cur_node != rhs.cur_node; }
        const value_type* operator->() const noexcept { return cur_node->data; }
    };

    map() : __root(nullptr), __end(new node()) { __begin = __end; }
    map(const map& other) {
        if (other.__root == nullptr) {
            __end   = new node;
            __begin = __end;
            __root  = nullptr;
            return;
        }
        __root = new node(other.__root->data, other.__root->height, other.__root->size);
        __end  = new node();
        ___ctor_inorder_traverse(__root, other.__root);
        node* proc = nullptr;
        //Connect all nodes with prev-next
        ___ctor_connect_all(__root, proc);
        proc->next  = __end;
        __end->prev = proc;
    }

    map& operator=(const map& other) {
        if (this == &other)
            return *this;
        this->~map();
        new (this) map(other);
        return *this;
    }

    ~map() {
        clear();
        if (__end != nullptr)
            delete __end;
    }
    //I'm blind. I didn't read the requirements.
    T& at(const Key& key) {
        node* p = __query_trav_(key, __root);
        if (p == nullptr)
            throw index_out_of_bound();
        return p->data->second;
    }

    const T at(const Key& key) const {
        node* p = __query_trav_(key, __root);
        if (p == nullptr)
            throw index_out_of_bound();
        return p->data->second;
    }

    //~~Cf. https://stackoverflow.com/questions/49287216/separate-handlers-lvalue-and-rvalue-operators ~~
    //pseudoref cannot pass the example
    T& operator[](const Key& key) {
        //Cf. https://github.com/LinsongGuo
        //! Thats why insert designed like this
        node* p = insert(pair<Key, T>(key, T())).first.cur_node;
        return (p->data->second);
    }
    const T& operator[](const Key& key) const {
        node* p = __query_trav_(key, __root);
        if (p == nullptr) {
            throw index_out_of_bound();
        }
        return p->data->second;
    }

    //now begin -> end() if empty
    iterator begin() { return iterator(__begin, this); }
    const_iterator cbegin() const { return const_iterator(__begin, this); }
    iterator end() { return iterator(__end, this); }
    const_iterator cend() const { return const_iterator(__end, this); }

    bool empty() const { return __root == nullptr; }
    size_t size() const { return __root == nullptr ? 0 : __root->size; }

    void clear() {
        ___dtor_postorder_traverse(__root);
        //! reinitialize the end ptr;
        delete __end;
        __end   = new node;
        __begin = __end;
        __root  = nullptr;
    }

    pair<iterator, bool> insert(const value_type& value) {
        node* p = __query_trav_(value.first, __root);
        if (p != nullptr)
            return pair<iterator, bool>(iterator(p, this), false);
        p = __add_entry(value, __root);
        if (__isleaf(__root)) {
            //reconnect
            __root->next = __end;
            __end->prev  = __root;
            __begin      = __root;
        }
        //Seems that the checker don't care the exception type.
        return pair<iterator, bool>(iterator(p, this), true);
    }

    void erase(iterator pos) {
        if (pos.mathis != this || pos.cur_node == nullptr || pos.cur_node->data == nullptr)
            throw invalid_iterator();
        //Seems that the checker don't care the exception type.
        __delete_entry(pos.cur_node, __root);
    }

    size_t count(const Key& key) const { return __query_trav_(key, __root) != nullptr; }

    iterator find(const Key& key) {
        node* p = __query_trav_(key, __root);
        return p == nullptr ? end() : iterator(p, this);
    }
    const_iterator find(const Key& key) const {
        node* p = __query_trav_(key, __root);
        return p == nullptr ? cend() : const_iterator(p, this);
    }
};
} // namespace sjtu
#endif
