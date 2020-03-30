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
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T>
class vector {
    //Give access to fast data processing between vectors.
    friend class vector;

private:
    //structure:
    //[0 1 2 3 4 5 6 7*] A 8-pr vector
    // ^-----------^.......serve as data section.
    //               ^.....serve as the full-vector flag.
    //           ^.........r_size==(5-0):the current end index.

    // capacity and size. Distinguished with option works.
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

    class const_iterator {
        friend class vector;

    private:
        const vector<T>* v;
        size_t delta;

    public:
        //legal for data access indicator
        bool legal;
        const_iterator() : v(nullptr), delta(0), legal(false){};

        const_iterator(iterator& it) : v(it.v), delta(it.delta), legal(it.legal){};
        const_iterator(const_iterator& it) : v(it.v), delta(it.delta), legal(it.legal){};
        //solving l-value problem
        const_iterator(iterator&& it) : v(it.v), delta(it.delta), legal(it.legal){};
        const_iterator(const_iterator&& it) : v(it.v), delta(it.delta), legal(it.legal){};

        const_iterator(const vector<T>* vec) : v(vec), delta(0), legal(true){};
        const_iterator(const vector<T>* vec, size_t dlt, bool le) : v(vec), delta(dlt), legal(le){};
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
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

        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
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
    iterator begin() { return iterator(this); }
    const_iterator cbegin() const { return const_iterator(this); }
    iterator end() { return iterator(this, r_size, false); }
    const_iterator cend() const { return const_iterator(this, r_size, false); }

    //* Create and Delete.
    //! MEMCPY may unable to process std::vector!
    //Damn C++! Why not just open a C data-structure course?
    vector() : r_capacity(__INIT_CAPACITY__), container((T*)malloc(sizeof(T) * r_capacity)), r_size(0) {
        //not setting to 0 causing bint error
        memset(container, 0, sizeof(T) * r_capacity);
        return;
    }
    vector(size_t init_size) {
        int size = 8;
        while (size < init_size)
            size <<= 1;
        r_capacity = size;
        r_size     = 0;
        container  = malloc(sizeof(T) * r_capacity);
        memset(container, 0, sizeof(T) * r_capacity);
        return;
    }
    vector(const vector& other) : r_capacity(other.capacity()), container((T*)malloc(sizeof(T) * r_capacity)), r_size(other.size()) {
        memset(container, 0, sizeof(T) * r_capacity);
        for (int i = 0; i < r_size; i++)
            container[i] = other.container[i];
        return;
    }
    ~vector() {
        free(container);
    }

    vector& operator=(const vector& other) {
        // avoid self assigning.
        if (&other == this)
            return *this;
        if (r_capacity >= other.capacity()) {
            //no realloc to save time.
            for (int i = 0; i < other.size(); i++)
                container[i] = other.container[i];
            r_size = other.size();
        } else {
            r_capacity = other.capacity();
            r_size     = other.size();
            free(container);
            container = (T*)malloc(sizeof(T) * r_capacity);
            memset(container, 0, sizeof(T) * r_capacity);
            for (int i = 0; i < r_size; i++)
                container[i] = other.container[i];
        }
        return *this;
    }

    // vector.at[index].
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

    bool empty() const { return r_size == 0; }
    size_t size() const { return r_size; }
    void clear() { r_size = 0; }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T& value) {
        if (r_size + 1 == r_capacity) {
            enlarge();
        }
        if (pos.legal) {
            for (int i = 0; i < r_size - pos.delta; i++) {
                *(pos + r_size - i) = *(pos + r_size - i - 1);
            }
            *pos = value;
            r_size++;
            return pos;
        } else {
            // end position
            push_back(value);
            return pos;
        }
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t& ind, const T& value) {
        return insert(iterator(*this, ind), value);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
        if (r_size == 0)
            throw container_is_empty();
        if (!pos.legal)
            throw index_out_of_bound();
        for (size_t i = 0; i < r_size - pos.delta - 1; i++) {
            *pos = *(&*pos + 1);
        }
        r_size--;
        return pos;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t& ind) {
        return erase(iterator(*this, ind));
    }
    /**
     * adds an element to the end.
     */

    void push_back(const T& value) {
        if (r_size == r_capacity - 1) {
            //full
            enlarge();
        }
        container[r_size] = value;
        r_size++;
    }
    void enlarge() {
        //enlarge the space.
        T* temp = (T*)malloc(sizeof(T) * r_capacity << 1);
        memset(temp, 0, sizeof(T) * r_capacity << 1);
        for (int i = 0; i < r_size; i++) {
            temp[i] = container[i];
        }
        free(container);
        container = temp;
        r_capacity <<= 1;
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if (r_size == 0)
            throw container_is_empty();
        else
            r_size--;
    }
    size_t capacity() const {
        return r_capacity;
    }
};

} // namespace sjtu

#endif
