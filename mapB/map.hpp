/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include "exceptions.hpp"
#include "utility.hpp"
#include <cmath>
#include <cstddef>
#include <functional>
namespace sjtu {

template <
    class Key,
    class T,
    class Compare = std::less<Key>>
class map {
    friend class AVL;

private:
    class AVL {

        friend class iterator;
        friend class const_iterator;

    public:
        /*
        * struct type_avl
        * used as a key-value 
        */
        struct type_avl : public sjtu::pair<const Key, T> {
            type_avl(const sjtu::pair<const Key, T>& other) : sjtu::pair<const Key, T>(other){};
            type_avl(sjtu::pair<const Key, T>&& other) : sjtu::pair<const Key, T>(other){};
            type_avl(const Key& x, const T& y) : sjtu::pair<const Key, T>(x, y) {}
            template <class U1, class U2>
            type_avl(U1&& x, U2&& y) : sjtu::pair<const Key, T>(x, y) {}
            template <class U1, class U2>
            type_avl(const pair<U1, U2>& other) : sjtu::pair<const Key, T>(other) {}
            template <class U1, class U2>
            type_avl(pair<U1, U2>&& other) : sjtu::pair<const Key, T>(other) {}
            //* fixed typo
            bool operator>(const type_avl& other) const {
                return !Compare()(this->first, other.first);
            }
            bool operator>=(const type_avl& other) const {
                return !(Compare()(other.first, this->first));
            }
            bool operator<(const type_avl& other) const {
                return Compare()(this->first, other.first);
            }
            bool operator<=(const type_avl& other) const {
                return (Compare()(other.first, this->first));
            }
            bool operator==(const type_avl& other) const {
                return (!Compare()(other.first, this->first)) && !Compare()(this->first, other.first);
            }
            bool operator!=(const type_avl& other) const {
                return (Compare()(other.first, this->first)) || Compare()(this->first, other.first);
            }
        };

        struct node {
            node* left;
            node* right;
            int height;
            type_avl value;
        };

        node* __data;
        size_t __size;
        //Balance factor
        inline int __get_factor(node*& root) const {
            if (root == nullptr)
                return 0;
            return (root->left == nullptr ? 0 : root->left->height) - (root->right == nullptr ? 0 : root->right->height);
        }
        //root height update
        inline void __set_root_height(node* root) {
            if (root == nullptr)
                return;
            root->height = std::max((root->left == nullptr ? 0 : root->left->height), (root->right == nullptr ? 0 : root->right->height)) + 1;
        }

        //AVL rotate
        node* __right_rotate(node*& root) {
            node* nl   = root->left;
            root->left = nl->right;
            nl->right  = root;
            root       = nl;
            __set_root_height(root->left);
            __set_root_height(root->right);
            __set_root_height(root);
            return root;
        }
        node* __left_rotate(node*& root) {
            node* nr    = root->right;
            root->right = nr->left;
            nr->left    = root;
            root        = nr;
            __set_root_height(root->left);
            __set_root_height(root->right);
            __set_root_height(root);
            return root;
        }
        //AVL balance
        int __balance(node*& root) {
            if (root == nullptr)
                return 0;
            __set_root_height(root->left);
            __set_root_height(root->right);
            __set_root_height(root);
            while (abs(__get_factor(root->left)) > 1) {
                __balance(root->left);
                __set_root_height(root->left);
                __set_root_height(root);
            }
            while (abs(__get_factor(root->right)) > 1) {
                __balance(root->right);
                __set_root_height(root->right);
                __set_root_height(root);
            }
            int p = __get_factor(root);
            if (abs(p) <= 1)
                return 0;
            if (p > 0) {
                if (__get_factor(root->left) == 1) {
                    __right_rotate(root);
                    return 0;
                } else {
                    __left_rotate(root->left);
                    __right_rotate(root);
                    return 0;
                }
            } else {
                if (__get_factor(root->right) == -1) {
                    __left_rotate(root);
                    return 0;
                } else {
                    __right_rotate(root->right);
                    __left_rotate(root);
                    return 0;
                }
            }
        }
        //find the specific storage node with Key to_query
        node* __query_trav_(node* root, const Key& to_query) const {
            if (root == nullptr)
                return nullptr;
            if ((!Compare()(root->value.first, to_query)) && (!Compare()(to_query, root->value.first)))
                return root;
            if (Compare()(root->value.first, to_query)) {
                return __query_trav_(root->right, to_query);
            } else {
                return __query_trav_(root->left, to_query);
            }
        }
        //find the specific storage node with a type_avl.
        node* __query_trav_(node* root, const type_avl& to_query) const {
            return __query_trav_(root, to_query.first);
        }
        //find the next bigger one
        node* __query_trav_big_(node* root, Key& to_query) const {
            //* Rewriten. Logical problem.
            if (root == nullptr)
                return nullptr;
            if (!Compare()(to_query, root->value.first)) {
                return __query_trav_big_(root->right, to_query);
            } else {
                node* ccmp = __query_trav_big_(root->left, to_query);
                if (ccmp == nullptr)
                    return root;
                if (Compare()(to_query, ccmp->value.first)) {
                    return ccmp;
                } else {
                    return root;
                }
            }
            return nullptr;
        }
        //find the next smaller one
        node* __query_trav_small_(node* root, Key& to_query) const {
            //* Rewriten. Logical problem.
            if (root == nullptr)
                return nullptr;
            if (!Compare()(root->value.first, to_query)) {
                return __query_trav_small_(root->right, to_query);
            } else {
                node* ccmp = __query_trav_small_(root->left, to_query);
                if (ccmp == nullptr)
                    return root;
                if (Compare()(ccmp->value.first, to_query)) {
                    return ccmp;
                } else {
                    return root;
                }
            }
            return nullptr;
        }
        //find the smallest one
        node* __query_trav_min_(node* root) const {
            if (root == nullptr)
                return nullptr;
            if (root->left != nullptr) {
                return __query_trav_min_(root->left);
            } else {
                return root;
            }
        }
        //find the biggest one
        node* __query_trav_max_(node* root) const {
            if (root == nullptr)
                return nullptr;
            if (root->right != nullptr) {
                return __query_trav_max_(root->right);
            } else {
                return root;
            }
        }
        //Add an entry.
        int __add_entry(node*& root, type_avl& to_insert) {
            if (root == nullptr) {
                root = new node{nullptr, nullptr, 1, to_insert};
                __size++;
                return 0;
            }
            if (root->value < to_insert)
                __add_entry(root->right, to_insert);
            else
                __add_entry(root->left, to_insert);
            __balance(root);
            return 0;
        }
        /* Performed in erase
    ** erase the minimum and return the minumum.
    */
        node* __erase_minimum(node*& root) {
            if (root == nullptr)
                return nullptr;
            node* to_erase;
            if (root->left == nullptr) {
                to_erase = root;
                root     = root->right;
            } else {
                return __erase_minimum(root->left);
            }
            __balance(root);
            return to_erase;
        }
        //delete an entry with key.
        int __delete_entry(node*& root, Key& to_delete) {
            if (root == nullptr)
                return -1;
            if ((!Compare()(root->value.first, to_delete)) && (!Compare()(to_delete, root->value.first))) {
                __size--;
                node* rr = root;
                if (root->right == nullptr && root->left == nullptr) {
                    root = nullptr;
                } else if (root->right == nullptr) {
                    root = root->left;
                } else if (root->left == nullptr) {
                    root = root->right;
                } else {
                    node*& pp   = root->right;
                    node* bl    = root->left;
                    root        = __erase_minimum(root->right);
                    root->right = pp;
                    root->left  = bl;
                }
                delete rr;
            } else if (Compare()(root->value.first, to_delete))
                __delete_entry(root->right, to_delete);
            else
                __delete_entry(root->left, to_delete);
            __balance(root);
            return 0;
        }
        //delete an entry with type_avl
        int __delete_entry(node*& root, type_avl& to_delete) {
            return __delete_entry(root, to_delete.first);
        }
        //traverse when copying
        int ___ctor_inorder_traverse(node* root, AVL& other) const {
            if (root == nullptr)
                return 0;
            if (root->left != nullptr) {
                ___ctor_inorder_traverse(root->left, other);
            }
            other.insert(root->value);
            if (root->right != nullptr) {
                ___ctor_inorder_traverse(root->right, other);
            }
            return 0;
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
        node* __next_node(node* n) const {
            return __query_trav_big_(__data, n->first);
        }

    public:
        AVL() : __data(nullptr), __size(0){};
        //Copy. (by modified insertion)
        //* Need to initiate an empty tree first.
        AVL(const AVL& a) : __data(nullptr), __size(0) {
            ___ctor_inorder_traverse(a.__data, *this);
            return;
        };
        AVL(const AVL&& a) : __data(nullptr), __size(0) {
            ___ctor_inorder_traverse(a.__data, *this);
            return;
        };
        ~AVL() {
            ___dtor_postorder_traverse(__data);
            return;
        }
        //insertion.
        int insert(type_avl& dat) {
            if (__query_trav_(__data, dat) != nullptr)
                throw sjtu::index_out_of_bound();
            else {
                __add_entry(__data, dat);
                return 0;
            }
        }
        int insert(type_avl&& dat) {
            if (__query_trav_(__data, dat) != nullptr)
                throw sjtu::index_out_of_bound();
            else {
                __add_entry(__data, dat);
                return 0;
            }
        }
        int insert(Key k, T t) {
            return insert(type_avl(k, t));
        }
        int insert(sjtu::pair<const Key, T>& p) {
            return insert((type_avl)p);
        }
        // * Edit: redesigning operator[]
        T& operator[](const Key& K) {
            node* n = __query_trav_(__data, K);
            if (n == nullptr) {
                //!To modify
                insert(K, T());
                return __query_trav_(__data, K)->value.second;
            }
            return n->value.second;
        }
        T operator[](const Key& K) const {
            node* n = __query_trav_(__data, K);
            if (n == nullptr)
                throw index_out_of_bound();
            return n->value.second;
        }
        bool exist(type_avl dat) const {
            return (__query_trav_(__data, dat) != nullptr);
        }
        bool exist(Key K) const {
            return (__query_trav_(__data, K) != nullptr);
        }
        //erase
        int erase(type_avl dat) {
            if (__query_trav_(__data, dat) == nullptr)
                return -1;
            else {
                __delete_entry(__data, dat);
                return 0;
            }
        }
        int erase(Key k) {
            if (__query_trav_(__data, k) == nullptr)
                return -1;
            else {
                __delete_entry(__data, k);
                return 0;
            }
        }
        node* _find(Key k) const {
            return __query_trav_(__data, k);
        }
        node* _next_key(Key k) const {
            return __query_trav_big_(__data, k);
        }
        node* _previous_key(Key k) const {
            return __query_trav_small_(__data, k);
        }
        node* _first() const {
            return __query_trav_min_(__data);
        }
        node* _last() const {
            return __query_trav_max_(__data);
        }
        int size() const {
            return __size;
        }
    };
    AVL tree;

public:
    typedef sjtu::pair<const Key, T> value_type;
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;
    class iterator {
        friend class map;

    private:
        map<Key, T, Compare>* mathis;
        Key* key;
        //0: start-1, 1 end
        bool endflag;

    public:
        iterator() : mathis(nullptr), key(nullptr), endflag(true) {}
        iterator(map<Key, T, Compare>* mathis, Key nodes) : mathis(mathis), key(new Key(nodes)), endflag(false){};
        iterator(map<Key, T, Compare>* mathis, Key nodes, bool endf) : mathis(mathis), key(new Key(nodes)), endflag(endf){};
        iterator(map<Key, T, Compare>* mathis, Key* nodes) : mathis(mathis), key(nodes), endflag(false){};
        iterator(map<Key, T, Compare>* mathis, Key* nodes, bool endf) : mathis(mathis), key(nodes), endflag(endf){};
        iterator(const iterator& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        iterator(const const_iterator& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        iterator(const iterator&& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        iterator(const const_iterator&& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        ~iterator() {
            if (key != nullptr)
                delete key;
        }

        iterator& operator=(const iterator& it) {
            if (this == &it)
                return *this;
            mathis = it.mathis;
            if (key != nullptr)
                delete key;
            //Fix logic error
            if (it.key != nullptr)
                key = new Key(*it.key);
            else
                key = nullptr;
            endflag = it.endflag;
            return *this;
        }
        iterator operator++(int) {
            iterator it(*this);
            ++*this;
            return it;
        }
        iterator& operator++() {
            if (key == nullptr || mathis == nullptr)
                throw invalid_iterator();
            auto* nodesa = mathis->tree._next_key(*key);
            if (nodesa == nullptr) {
                delete key;
                key     = nullptr;
                endflag = 1;
            } else {
                delete key;
                key = new Key(nodesa->value.first);
            }
            return *this;
        }
        iterator operator--(int) {
            iterator it(*this);
            --*this;
            return it;
        }
        iterator& operator--() {
            if (mathis == nullptr)
                throw invalid_iterator();
            typename AVL::node* nodesa;
            if (key == nullptr && endflag != 1) {
                throw invalid_iterator();
            } else if (endflag == 1) {
                nodesa = mathis->tree._last();
            } else {
                nodesa = mathis->tree._previous_key(*key);
            }
            if (nodesa == nullptr) {
                delete key;
                key     = nullptr;
                endflag = 0;
            } else {
                delete key;
                key = new Key(nodesa->value.first);
            }
            return *this;
        }
        value_type& operator*() const {
            if (key == nullptr || mathis == nullptr)
                throw index_out_of_bound();
            return mathis->tree._find(*key)->value;
        }
        bool operator==(const iterator& rhs) const {
            if (mathis != rhs.mathis)
                return false;
            if (key == nullptr || rhs.key == nullptr)
                return (key == rhs.key);
            return ((!Compare()(*key, *(rhs.key))) && (!Compare()(*(rhs.key), *key)));
        }
        bool operator==(const const_iterator& rhs) const {
            if (mathis != rhs.mathis)
                return false;
            if (key == nullptr || rhs.key == nullptr)
                return (key == rhs.key);
            return ((!Compare()(*key, *(rhs.key))) && (!Compare()(*(rhs.key), *key)));
        }
        bool operator!=(const iterator& rhs) const {
            if (mathis != rhs.mathis)
                return true;
            if (key == nullptr || rhs.key == nullptr)
                return !(key == rhs.key);
            (Compare()(*key, *(rhs.key))) || (!Compare()(*(rhs.key), *key));
        }
        bool operator!=(const const_iterator& rhs) const {
            if (mathis != rhs.mathis)
                return true;
            if (key == nullptr || rhs.key == nullptr)
                return !(key == rhs.key);
            return (Compare()(*key, *(rhs.key))) || (!Compare()(*(rhs.key), *key));
        }
        value_type* operator->() const noexcept {
            return &*(*this);
        }
    };
    class const_iterator {
        friend class map;

    private:
        const map<Key, T, Compare>* mathis;
        Key* key;
        //0: start-1, 1 end
        bool endflag;

    public:
        const_iterator() : mathis(nullptr), key(nullptr), endflag(true) {}
        const_iterator(const map<Key, T, Compare>* mathis, Key nodes) : mathis(mathis), key(new Key(nodes)), endflag(false){};
        const_iterator(const map<Key, T, Compare>* mathis, Key nodes, bool endf) : mathis(mathis), key(new Key(nodes)), endflag(endf){};
        const_iterator(const map<Key, T, Compare>* mathis, Key* nodes) : mathis(mathis), key(nodes), endflag(false){};
        const_iterator(const map<Key, T, Compare>* mathis, Key* nodes, bool endf) : mathis(mathis), key(nodes), endflag(endf){};
        const_iterator(const iterator& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        const_iterator(const const_iterator& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        const_iterator(const iterator&& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        const_iterator(const const_iterator&& other) : mathis(other.mathis), endflag(other.endflag) {
            if (other.key != nullptr) {
                key = new Key(*other.key);
            } else {
                key = nullptr;
            }
        }
        ~const_iterator() {
            if (key != nullptr)
                delete key;
        }

        const_iterator& operator=(const const_iterator& it) {
            if (this == &it)
                return *this;
            mathis = it.mathis;
            if (key != nullptr)
                delete key;
            if (it.key != nullptr)
                key = new Key(*it.key);
            else
                key = nullptr;
            endflag = it.endflag;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator it(*this);
            ++*this;
            return it;
        }
        const_iterator& operator++() {
            if (key == nullptr || mathis == nullptr)
                throw invalid_iterator();
            auto nodesa = mathis->tree._next_key(*key);
            if (nodesa == nullptr) {
                delete key;
                key     = nullptr;
                endflag = 1;
            } else {
                delete key;
                key = new Key(nodesa->value.first);
            }
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator it(*this);
            --*this;
            return it;
        }
        const_iterator& operator--() {
            if (mathis == nullptr)
                throw invalid_iterator();
            typename AVL::node* nodesa;
            if (key == nullptr && endflag != 1) {
                throw invalid_iterator();
            } else if (endflag == 1) {
                nodesa = mathis->tree._last();
            } else {
                nodesa = mathis->tree._previous_key(*key);
            }
            if (nodesa == nullptr) {
                delete key;
                key     = nullptr;
                endflag = 0;
            } else {
                delete key;
                key = new Key(nodesa->value.first);
            }
            return *this;
        }
        const value_type& operator*() const {
            if (key == nullptr || mathis == nullptr)
                throw index_out_of_bound();
            return mathis->tree._find(*key)->value;
        }
        //Fixed. Different iterators
        bool operator==(const iterator& rhs) const {
            if (mathis != rhs.mathis)
                return false;
            if (key == nullptr || rhs.key == nullptr)
                return (key == rhs.key);
            return !((!Compare()(*key, *(rhs.key))) && (!Compare()(*(rhs.key), *key)));
        }
        bool operator==(const const_iterator& rhs) const {
            if (mathis != rhs.mathis)
                return false;
            if (key == nullptr || rhs.key == nullptr)
                return (key == rhs.key);
            return !((!Compare()(*key, *(rhs.key))) && (!Compare()(*(rhs.key), *key)));
        }
        bool operator!=(const iterator& rhs) const {
            if (mathis != rhs.mathis)
                return true;
            if (key == nullptr || rhs.key == nullptr)
                return !(key == rhs.key);
            return ((!Compare()(*key, *(rhs.key))) && (!Compare()(*(rhs.key), *key)));
        }
        bool operator!=(const const_iterator& rhs) const {
            if (mathis != rhs.mathis)
                return true;
            if (key == nullptr || rhs.key == nullptr)
                return !(key == rhs.key);
            return ((!Compare()(*key, *(rhs.key))) && (!Compare()(*(rhs.key), *key)));
        }
        const value_type* operator->() const noexcept { return &*(*this); }
    };
    friend class const_iterator;
    friend class iterator;
    map() : tree() {}
    map(const map<Key, T, Compare>& other) : tree(other.tree) {}
    /**
     * TODO assignment operator
     */
    map& operator=(const map<Key, T, Compare>& other) {
        if (this == &other)
            return *this;
        (&tree)->~AVL();
        new (&tree) AVL(other.tree);
        return *this;
    }
    ~map() {}
    //I'm blind. I didn't read the requirements.
    T& at(const Key& key) {
        if (!tree._find(key))
            throw index_out_of_bound();
        return tree[key];
    }
    const T& at(const Key& key) const {
        if (!tree._find(key))
            throw index_out_of_bound();
        return tree[key];
    }
    //* modified
    T& operator[](const Key& key) { return tree[key]; }
    T operator[](const Key& key) const { return tree[key]; }
    iterator begin() {
        //To fit begin()==end(): aka empty()
        if (tree.__size == 0) {
            return end();
        }
        return iterator(this, tree._first()->value.first);
    }
    const_iterator cbegin() const {
        //To fit begin()==end(): aka empty()
        if (tree.__size == 0) {
            return cend();
        }
        return const_iterator(this, tree._first()->value.first);
    }
    iterator end() {
        return iterator(this, nullptr, true);
    }
    const_iterator cend() const {
        return const_iterator(this, nullptr, true);
    }
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const {
        return !(tree.size());
    }
    /**
     * returns the number of elements.
     */
    size_t size() const { return tree.size(); }
    /**
     * clears the contents
     */
    void clear() {
        (&tree)->~AVL();
        new (&tree) AVL();
        return;
    }
    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type& value) {
        //* changed design of iterator and[]
        auto n = tree._find(value.first);
        if (n != nullptr)
            return pair<iterator, bool>(iterator(this, n->value.first), false);
        tree.insert(value.first, value.second);
        return pair<iterator, bool>(iterator(this, value.first), true);
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator& pos) {
        if (pos.key == nullptr || pos.mathis != this)
            throw invalid_iterator();
        try {
            tree.erase(*(pos.key));
        } catch (...) {
            throw invalid_iterator();
        }
    }
    void erase(iterator&& pos) {
        if (pos.key == nullptr || pos.mathis != this)
            throw invalid_iterator();
        try {
            tree.erase(*(pos.key));
        } catch (...) {
            throw invalid_iterator();
        }
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key& key) const {
        return tree.exist(key);
    }
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key& key) {
        auto n = tree._find(key);
        if (n != nullptr) {
            return iterator(this, key);
        } else {
            return end();
        }
    }
    const_iterator find(const Key& key) const {
        auto n = tree._find(key);
        if (n != nullptr) {
            return const_iterator(this, key);
        } else {
            return cend();
        }
    }
};

} // namespace sjtu

#endif
