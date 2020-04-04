#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

#include <cstring>
using std::memcpy;

#define __INIT_CAPACITY__ 8
#define __INCREASE_SIZE__ 2

namespace sjtu {
//like std::vector.
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
            new(container+i) T(other.container[i]);
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
                new(container+i) T(other.container[i]);
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
            new(temp+i) T(container[i]);
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
            new(temp_container+i) T(container[i]);
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

} // namespace sjtu

#endif
