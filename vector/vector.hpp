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
    // Data Container
    T* container;
    // capacity and size. Distinguished with option works.
    size_t r_capacity;
    size_t r_size;

public:
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;
    class iterator {
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
    public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator operator+(const int& n) const {
            //TODO
        }
        iterator operator-(const int& n) const {
            //TODO
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const iterator& rhs) const {
            //TODO
        }
        iterator& operator+=(const int& n) {
            //TODO
        }
        iterator& operator-=(const int& n) {
            //TODO
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {}
        /**
         * TODO ++iter
         */
        iterator& operator++() {}
        /**
         * TODO iter--
         */
        iterator operator--(int) {}
        /**
         * TODO --iter
         */
        iterator& operator--() {}
        /**
         * TODO *it
         */
        T& operator*() const {}
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator& rhs) const {}
        bool operator==(const const_iterator& rhs) const {}
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator& rhs) const {}
        bool operator!=(const const_iterator& rhs) const {}
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
    };
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

    vector() : r_capacity(__INIT_CAPACITY__), r_size(0), container(new T[r_capacity]) { return; }
    vector(size_t init_size) {
        int size = 8;
        while (size < init_size)
            size <<= 1;
        r_capacity = size;
        r_size     = 0;
        container  = new T[r_capacity];
        return;
    }
    vector(const vector& other) : r_capacity(other.capacity()), r_size(other.size()), container(new T[r_capacity]) {
        memcpy(other.container, container, sizeof(T) * r_size());
        return;
    }
    ~vector() {
        delete[] container;
    }

    vector& operator=(const vector& other) {
        // avoid self assigning.
        if (other == *this)
            return *this;
        if (r_capacity >= other.capacity()) {
            //direct copy to save time.
            memcpy(other.container, container, other.size() * sizeof(T));
        } else {
            r_capacity = other.capacity();
            r_size     = other.size();
            delete[] container;
            container = new T[r_capacity];
            memcpy(other.container, container, other.size() * sizeof(T));
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
    iterator insert(iterator pos, const T& value) {}
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t& ind, const T& value) {}
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {}
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t& ind) {}
    /**
     * adds an element to the end.
     */
    void push_back(const T& value) {
        if(r_size==r_capacity-1){
            //full
            
        }
    }
    void enlarge(){
        //auto enlarge the space.
        for()
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
};

} // namespace sjtu

#endif
