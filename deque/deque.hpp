#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

#include <cstring>
using std::memcpy;

//vector
#define __INIT_CAPACITY__ 8
#define __INCREASE_SIZE__ 2

namespace sjtu {

// last time's vector
template <typename T>
class vector {
    //Give access to fast data processing between vectors.
    friend class vector;

private:
    //* Main Structure:
    //[0 1 2 3 4 5 6 7*] A 8-pr vector
    // ^-----------^.......serve as data section.
    //               ^.....serve as the full-vector flag.
    //           ^.........r_size==(5-0):the current end index.

    //Distinguished with option works.
    size_t r_capacity;
    // Data Container
    T* container;
    size_t r_size;

public:
    class const_iterator;
    class iterator {
        friend class vector;

    private:
        vector<T>* v;
        size_t delta;

    public:
        //legal for data access indicator
        bool legal;

        //default iterator initiator.
        iterator() : v(nullptr), delta(0), legal(false){};
        //copy initiator.
        iterator(iterator& it) : v(it.v), delta(it.delta), legal(it.legal){};
        iterator(const_iterator& it) : v(it.v), delta(it.delta), legal(it.legal){};
        //solving l-value problem.
        iterator(iterator&& it) : v(it.v), delta(it.delta), legal(it.legal){};
        iterator(const_iterator&& it) : v(it.v), delta(it.delta), legal(it.legal){};
        //useful initiators.
        iterator(vector<T>* vec) : v(vec), delta(0), legal(true){};
        iterator(vector<T>* vec, size_t dlt) : v(vec), delta(dlt) {
            // legal for data access
            if (dlt == vec->r_size)
                legal = false;
            else
                legal = true;
        };
        //full iterator initiator.
        iterator(vector<T>* vec, size_t dlt, bool le) : v(vec), delta(dlt), legal(le){};

        iterator operator+(const int& n) const {
            // check if bound valid.
            if (delta + n < 0 || delta + n > v->r_size)
                throw index_out_of_bound();
            // forbid vector.end() access.
            if (delta + n == v->r_size)
                return iterator(v, delta + n, false);
            else
                return iterator(v, delta + n, true);
        }
        //conjunctive
        iterator operator-(const int& n) const {
            return *this + (-n);
        }

        int operator-(const iterator& rhs) const {
            //different vectors shall throw invaild_iterator.
            if (v != rhs.v)
                throw invalid_iterator();
            return delta - rhs.delta;
        }
        iterator& operator+=(const int& n) {
            // check if bound valid.
            if (delta + n < 0 || delta + n > v->r_size)
                throw index_out_of_bound();
            // forbid vector.end() access, removing limit on others.
            if (delta + n == v->r_size)
                legal = false;
            else
                legal = true;

            delta += n;
            return *this;
        }
        //conjunctive
        iterator& operator-=(const int& n) {
            return *this += (-n);
        }

        iterator operator++(int) {
            // check if bound valid.
            if (delta + 1 < 0 || delta + 1 > v->r_size)
                throw index_out_of_bound();
            iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta + 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta += 1;
            return temp;
        }

        iterator& operator++() {
            if (delta + 1 < 0 || delta + 1 > v->r_size)
                throw index_out_of_bound();
            iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta + 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta += 1;
            return *this;
        }

        iterator operator--(int) {
            // check if bound valid.
            if (delta - 1 < 0 || delta - 1 > v->r_size)
                throw index_out_of_bound();
            iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta - 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta -= 1;
            return temp;
        }

        iterator& operator--() {
            if (delta - 1 < 0 || delta - 1 > v->r_size)
                throw index_out_of_bound();
            iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta - 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta -= 1;
            return *this;
        }

        T& operator*() const {
            // forbid vector.end() access
            if (legal)
                return (*v)[delta];
            else
                throw index_out_of_bound();
        }

        bool operator==(const iterator& rhs) const {
            return (rhs.delta == delta && rhs.v == v);
        }
        bool operator==(const const_iterator& rhs) const {
            return (rhs.delta == delta && rhs.v == v);
        }

        bool operator!=(const iterator& rhs) const {
            return !(rhs.delta == delta && rhs.v == v);
        }
        bool operator!=(const const_iterator& rhs) const {
            return !(rhs.delta == delta && rhs.v == v);
        }
    };
    //Just nearly the same, with specific access control modified.
    class const_iterator {
        friend class vector;

    private:
        const vector<T>* v;
        size_t delta;

    public:
        //* EXTRA COMMENT Cf. class iterator.
        bool legal;
        const_iterator() : v(nullptr), delta(0), legal(false){};

        const_iterator(iterator& it) : v(it.v), delta(it.delta), legal(it.legal){};
        const_iterator(const_iterator& it) : v(it.v), delta(it.delta), legal(it.legal){};
        //solving l-value problem
        const_iterator(iterator&& it) : v(it.v), delta(it.delta), legal(it.legal){};
        const_iterator(const_iterator&& it) : v(it.v), delta(it.delta), legal(it.legal){};

        const_iterator(const vector<T>* vec) : v(vec), delta(0), legal(true){};
        const_iterator(const vector<T>* vec, size_t dlt, bool le) : v(vec), delta(dlt), legal(le){};

        const_iterator operator+(const int& n) const {
            // check if bound valid.
            if (delta + n < 0 || delta + n > v->r_size)
                throw index_out_of_bound();
            // forbid vector.end() access.
            if (delta + n == v->r_size)
                return const_iterator(v, delta + n, false);
            else
                return const_iterator(v, delta + n, true);
        }
        const_iterator operator-(const int& n) const {
            return *this + (-n);
        }

        int operator-(const const_iterator& rhs) const {
            if (v != rhs.v)
                throw invalid_iterator();
            return delta - rhs.delta;
        }
        const_iterator& operator+=(const int& n) {
            // check if bound valid.
            if (delta + n < 0 || delta + n > v->r_size)
                throw index_out_of_bound();
            // forbid vector.end() access, removing limit on others.
            if (delta + n == v->r_size)
                legal = false;
            else
                legal = true;

            delta += n;
            return *this;
        }
        const_iterator& operator-=(const int& n) {
            return *this += (-n);
        }

        const_iterator operator++(int) {
            // check if bound valid.
            if (delta + 1 < 0 || delta + 1 > v->r_size)
                throw index_out_of_bound();
            const_iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta + 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta += 1;
            return temp;
        }

        const_iterator& operator++() {
            if (delta + 1 < 0 || delta + 1 > v->r_size)
                throw index_out_of_bound();
            const_iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta + 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta += 1;
            return *this;
        }

        const_iterator operator--(int) {
            // check if bound valid.
            if (delta - 1 < 0 || delta - 1 > v->r_size)
                throw index_out_of_bound();
            const_iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta - 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta -= 1;
            return temp;
        }

        const_iterator& operator--() {
            if (delta - 1 < 0 || delta - 1 > v->r_size)
                throw index_out_of_bound();
            const_iterator temp(*this);
            // forbid vector.end() access, removing limit on others.
            if (delta - 1 == v->r_size)
                legal = false;
            else
                legal = true;
            delta -= 1;
            return *this;
        }

        const T& operator*() const {
            // forbid vector.end() access
            if (legal)
                return (*v)[delta];
            else
                throw index_out_of_bound();
        }

        bool operator==(const iterator& rhs) const {
            return (rhs.delta == delta && rhs.v == v);
        }
        bool operator==(const const_iterator& rhs) const {
            return (rhs.delta == delta && rhs.v == v);
        }

        bool operator!=(const iterator& rhs) const {
            return !(rhs.delta == delta && rhs.v == v);
        }
        bool operator!=(const const_iterator& rhs) const {
            return !(rhs.delta == delta && rhs.v == v);
        }
    };
    //itertal access extra-initiator.
    iterator begin() { return iterator(this); }
    const_iterator cbegin() const { return const_iterator(this); }
    iterator end() { return iterator(this, r_size, false); }
    const_iterator cend() const { return const_iterator(this, r_size, false); }

    //* Create and Delete.
    //! MEMCPY may be unable to process std::vector! Using operator= instead.
    //Damn C++! Why not just open a C data-structure course?

    //default initiator.
    vector() : r_capacity(__INIT_CAPACITY__), container((T*)operator new[](sizeof(T) * r_capacity)), r_size(0) {
        //this is malloc's error. Malloc causes unpredicted memory leak.
        //reuse operator new[] instead.
        //Cf. https://zh.cppreference.com/w/cpp/memory/new/operator_new
        //That's interesting... I'm too weak (._.)rz
        //RENEW: I found errors not fit but passing the test.
        //now changed.
        //-------------obsolete----------------
        //not setting to 0 is causing bint error.
        //bint is comparing capacity to assign,
        //and unprocessed random bint.capacity value causes SIGSEGV.
        return;
    }

    //initating with an init_size.
    vector(size_t init_size) {
        // for the goodness of memory access,
        // sizes are fit to 8*2^n.
        size_t size = __INIT_CAPACITY__;
        // fitting init_size.
        while (size < init_size)
            size <<= 1;
        r_capacity = size;
        r_size     = 0;
        container  = (T*)operator new[](sizeof(T) * r_capacity);
        return;
    }

    //copy initiator.
    vector(const vector& other) : r_capacity(other.capacity()), container((T*)operator new[](sizeof(T) * r_capacity)), r_size(other.size()) {
        for (size_t i = 0; i < r_size; i++)
            new (container + i) T(other.container[i]);
        return;
    }

    //destroyer
    ~vector() {
        for (size_t i = 0; i < r_size; i++)
            container[i].~T();
        operator delete[](container, r_capacity * sizeof(T));
    }

    //Assignment. using equal to avoid directly copying from
    //memory causing unpredicted errors, such as std::vector error.
    vector& operator=(const vector& other) {
        // avoid self assigning.
        if (&other == this)
            return *this;
        if (r_capacity >= other.capacity()) {
            //no additional alloc to save time.
            //I have not add link to shrink().
            //That should belong to some daemon-level memory saving modules.
            for (size_t i = 0; i < other.size(); i++)
                container[i] = other.container[i];
            r_size = other.size();
        } else {
            //reallocate to fit the data.

            for (size_t i = 0; i < r_size; i++)
                container[i].~T();
            operator delete[](container, r_capacity * sizeof(T));
            r_capacity = other.capacity();
            r_size     = other.size();
            container  = (T*)operator new[](sizeof(T) * r_capacity);
            for (size_t i = 0; i < r_size; i++)
                new (container + i) T(other.container[i]);
        }
        return *this;
    }

    // vector.at[index] to access as vector[index]? maybe.
    T& at(const size_t& pos) {
        if (pos < 0 || pos >= r_size) {
            throw index_out_of_bound();
        }
        return container[pos];
    }
    const T& at(const size_t& pos) const {
        if (pos < 0 || pos >= r_size) {
            throw index_out_of_bound();
        }
        return container[pos];
    }
    T& operator[](const size_t& pos) { return at(pos); }
    const T& operator[](const size_t& pos) const { return at(pos); }

    //front and back elements access
    const T& front() const {
        if (r_size == 0)
            throw container_is_empty();
        return *container;
    }
    const T& back() const {
        if (r_size == 0)
            throw container_is_empty();
        return *(container + r_size - 1);
    }

    //check if empty.
    bool empty() const { return r_size == 0; }
    //size access.
    size_t size() const { return r_size; }
    //Damn. No time shall be saved.
    void clear() {
        for (size_t i = 0; i < r_size; i++) {
            container[i].~T();
        }
        r_size = 0;
    }

    //[0 1 2 3 4 5 6]
    //       ^--------insert():
    //[0 1 2 9 3 4 5 6]
    //       ^--------returned iterator.
    iterator insert(iterator pos, const T& value) {
        if (r_size + 1 == r_capacity) {
            //if vector full of elements.
            enlarge();
        }
        if (pos.legal) {
            for (size_t i = 0; i < r_size - pos.delta; i++)
                *(container + r_size - i) = *(container + r_size - i - 1);
            *pos = value;
            r_size++;
            return pos;
        } else {
            // end position, call push_back.
            push_back(value);
            return pos;
        }
    }
    // alias
    iterator insert(const size_t& ind, const T& value) {
        return insert(iterator(*this, ind), value);
    }

    //delete selected element and return the next.
    iterator erase(iterator pos) {
        if (r_size == 0)
            // forbid empty vector access.
            throw container_is_empty();
        if (!pos.legal)
            // forbid vector.end() deletion.
            throw index_out_of_bound();
        for (size_t i = 0; i < r_size - pos.delta - 1; i++)
            *(container + pos.delta + i) = *(container + pos.delta + i + 1);
        container[--r_size].~T();
        return pos;
    }
    //alias
    iterator erase(const size_t& ind) {
        return erase(iterator(*this, ind));
    }

    //append
    void push_back(const T& value) {
        if (r_size == r_capacity - 1) {
            //if vector full of elements.
            enlarge();
        }
        //Cf. https://blog.csdn.net/wudaijun/article/details/9273339
        //THAT'S MAGIC
        new (container + r_size) T(value);
        r_size++;
    }
    void enlarge() {
        //enlarge the space.
        T* temp = (T*)operator new[](sizeof(T) * r_capacity << 1);
        for (size_t i = 0; i < r_size; i++) {
            new (temp + i) T(container[i]);
            //After malloc should destroy separately.
            //Cf. https://www.cnblogs.com/jobshunter/p/10976308.html
            container[i].~T();
        }
        operator delete[](container, r_capacity * sizeof(T));
        container = temp;
        r_capacity <<= 1;
    }
    void shrink() {
        //this function is isolated.
        //forbid extra-capasity shrink.
        if (r_size * 2 >= r_capacity)
            return;
        size_t temp = r_capacity;
        while (r_size * 2 < temp && temp >= 8)
            temp >>= 1;
        temp <<= 1;
        T* temp_container = (T*)operator new[](sizeof(T) * temp);
        for (size_t i = 0; i < r_size; i++) {
            new (temp_container + i) T(container[i]);
            container[i].~T();
        }
        operator delete[](container, r_capacity * sizeof(T));
        container  = temp_container;
        r_capacity = temp;
        return;
    }

    //remove the last element.
    void pop_back() {
        if (r_size == 0)
            throw container_is_empty();
        else
            container[--r_size].~T();
    }
    //return capacity.
    size_t capacity() const {
        return r_capacity;
    }
};

struct d_index {
    bool legal;
    int block;
    int subpos;
};

template <class T>
class deque {
    friend class deque;

private:
    //using a vector to contain all data.
    vector<vector<T>> container;
    size_t d_size;
    d_index query(int pos) {
        if (pos > d_size)
            throw(index_out_of_bound());
        if (pos == d_size) {
            return d_index{0, container.size(), container.back.size() + 1};
        }
        d_index ans{1, 0, 0};
        int sp = 0;
        while (sp < pos) {
            sp += container[ans.block].size();
            ++ans.block;
        }
        sp -= container[ans.block].size();
        ans.block--;
        ans.subpos = pos - sp;
        return ans;
    };
    // the function does what you think.
    void split(typename vector<vector<T>>::iterator it) {
        size_t _s = (*it).size() / 2;
        vector<T> back(_s);
        container.insert(it + 1, back(_s));
        for (auto ps = (*it).begin() + ((*it).size() - _s); ps != (*it).end(); ps = (*it).erase(ps))
            *(it + 1).push_back(*ps);
        return;
    }
    // merge it and it+1 UNSAFE
    void merge(typename vector<vector<T>>::iterator it) {
        for (auto ps = *(it + 1).begin(); ps != *(it + 1).end(); ps = *(it + 1).erase(ps))
            (*it).push_back(*ps);
        container.erase(it + 1);
        return;
    }
    void remove_empty_block() {
        for (auto ps = container.begin(); ps != container.end();) {
            if ((*ps).size() == 0) {
                container.erase(ps);
                continue;
            } else {
                ps++;
            }
        }
        return;
    }
    //TODO fit function
    void fit(){};

public:
    class const_iterator;
    class iterator {
    private:
    public:
        //* i0  here can be optimized with block-subposition
        deque<T>* deq;
        size_t delta;
        iterator(deque<T>* deqs, size_t delta) : deq(deqs), delta(delta){};
        iterator() : deq(nullptr), delta(0){};
        /**
         * return a new iterator which pointer n-next elements
         *   even if there are not enough elements, the behaviour is **undefined**.
         * as well as operator-
         */
        iterator operator+(const int& n) const {
            return iterator(this, delta + n);
        }
        iterator operator-(const int& n) const {
            return iterator(this, delta - n);
        }
        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const iterator& rhs) const {
            if (rhs.deq != deq)
                throw(invalid_iterator());
            return delta - rhs.delta;
        }
        iterator& operator+=(const int& n) {
            delta += n;
            return *this;
        }
        iterator& operator-=(const int& n) {
            delta -= n;
            return *this;
        }
        iterator operator++(int) {
            auto ps = *this;
            ++delta;
            return ps;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() {
            ++delta;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            auto ps = *this;
            --delta;
            return ps;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() {
            --delta;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const {
            return deq[delta];
        }
        /**
         * TODO it->field
         */
        T* operator->() const noexcept {
            return &deq[delta];
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator& rhs) const {
            return (deq == rhs.deq && delta == rhs.delta);
        }
        bool operator==(const const_iterator& rhs) const {
            return (deq == rhs.deq && delta == rhs.delta);
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator& rhs) const {
            return !(deq == rhs.deq && delta == rhs.delta);
        }
        bool operator!=(const const_iterator& rhs) const {
            return !(deq == rhs.deq && delta == rhs.delta);
        }
    };
    class const_iterator {
    private:
    public:
        deque<T>* deq;
        size_t delta;
        const_iterator(deque<T>* deqs, size_t delta) : deq(deqs), delta(delta){};
        const_iterator() : deq(nullptr), delta(0){};
        const_iterator(const const_iterator& other) : deq(other.deq), delta(other.delta){};
        const_iterator(const iterator& other) : deq(other.deq), delta(other.delta){};
        const_iterator operator+(const int& n) const {
            return const_iterator(this, delta + n);
        }
        const_iterator operator-(const int& n) const {
            return const_iterator(this, delta - n);
        }
        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const const_iterator& rhs) const {
            if (rhs.deq != deq)
                throw(invalid_iterator());
            return delta - rhs.delta;
        }
        const_iterator& operator+=(const int& n) {
            delta += n;
            return *this;
        }
        const_iterator& operator-=(const int& n) {
            delta -= n;
            return *this;
        }
        const_iterator operator++(int) {
            auto ps = *this;
            ++delta;
            return ps;
        }
        const_iterator& operator++() {
            ++delta;
            return *this;
        }
        const_iterator operator--(int) {
            auto ps = *this;
            --delta;
            return ps;
        }
        const_iterator& operator--() {
            --delta;
            return *this;
        }
        const T& operator*() const {
            return deq[delta];
        }
        const T* operator->() const noexcept {
            return &deq[delta];
        }
        bool operator==(const iterator& rhs) const {
            return (deq == rhs.deq && delta == rhs.delta);
        }
        bool operator==(const const_iterator& rhs) const {
            return (deq == rhs.deq && delta == rhs.delta);
        }
        bool operator!=(const iterator& rhs) const {
            return !(deq == rhs.deq && delta == rhs.delta);
        }
        bool operator!=(const const_iterator& rhs) const {
            return !(deq == rhs.deq && delta == rhs.delta);
        }
    };

    deque() : d_size(0) {}
    deque(const deque& other) {
        container = other.container;
        d_size    = other.d_size;
        fit();
    }
    ~deque() {}
    deque& operator=(const deque& other) {
        if (*other == this)
            return *this;
        container = other.container;
        d_size    = other.d_size;
        fit();
    }
    /**
     * access specified element with bounds checking
     * throw index_out_of_bound if out of bound.
     */
    T& at(const size_t& pos) {
        if (pos >= d_size)
            throw(index_out_of_bound());
        d_index d = query(pos);
        return container[d.block][d.subpos];
    }
    const T& at(const size_t& pos) const {
        if (pos >= d_size)
            throw(index_out_of_bound());
        d_index d = query(pos);
        return container[d.block][d.subpos];
    }
    T& operator[](const size_t& pos) {
        return at(pos);
    }
    const T& operator[](const size_t& pos) const {
        return at(pos);
    }
    /**
     * access the first element
     * throw container_is_empty when the container is empty.
     */
    const T& front() const {
        if (d_size == 0)
            throw(container_is_empty());
        remove_empty_block();
        return container[0][0];
    }
    /**
     * access the last element
     * throw container_is_empty when the container is empty.
     */
    const T& back() const {
        if (d_size == 0)
            throw(container_is_empty());
        remove_empty_block();
        return container.back().back();
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {}
    const_iterator cbegin() const {}
    /**
     * returns an iterator to the end.
     */
    iterator end() {}
    const_iterator cend() const {}
    /**
     * checks whether the container is empty.
     */
    bool empty() const {
        return !(d_size);
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return d_size;
    }
    /**
     * clears the contents
     */
    void clear() {
        if (d_size == 0)
            return;
        container.clear;
        d_size = 0;
        return;
    }
    /**
     * inserts elements at the specified locat on in the container.
     * inserts value before pos
     * returns an iterator pointing to the inserted value
     *     throw if the iterator is invalid or it point to a wrong place.
     */
    iterator insert(iterator pos, const T& value) {
        if (pos.deq != this || pos.delta > d_size || pos.delta < 0)
            throw(invalid_iterator());
        if (pos.delta == d_size) {
            push_back(value);
            return pos;
        }
        //* optimize i0
        d_index d = query(pos.delta);
        container[d.block].insert(d.subpos, value);
        return pos;
    }
    /**
     * removes specified element at pos.
     * removes the element at pos.
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid or it points to a wrong place.
     */
    iterator erase(iterator pos) {
        if (d_size == 0)
            throw(container_is_empty());
        if (pos.deq != this || pos.delta > d_size || pos.delta < 0)
            throw(invalid_iterator());
        //* optimize i0
        d_index d = query(pos.delta);
        container[d.block].erase(d.subpos);
        //* d1 whether it should clear empty here?
        return pos;
    }
    /**
     * adds an element to the end
     */
    void push_back(const T& value) {
        container.back().push_back(value);
    }
    /**
     * removes the last element
     *     throw when the container is empty.
     */
    void pop_back() {
        if (d_size == 0)
            throw(container_is_empty());
        remove_empty_block();
        container.back().pop_back();
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T& value) {
        //* d2 split within
        container.front().insert(0, value);
    }
    /**
     * removes the first element.
     *     throw when the container is empty.
     */
    void pop_front() {
        if (d_size == 0)
            throw(container_is_empty());
        remove_empty_block();
        container.front().erase(0);
    }
};

} // namespace sjtu

#endif
