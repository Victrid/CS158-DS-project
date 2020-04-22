#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

#ifndef __CHUCKSIZE__
#define __CHUCKSIZE__ 256
#endif

#ifndef __CHECKVALVE__
#define __CHECKVALVE__ 1
#endif

namespace sjtu {

template <typename T>
class deque {
private:
    struct node {
        T* data;
        node *last, *next;
        node() : data(nullptr), last(nullptr), next(nullptr) {}
        node(node* last, node* next) : data(nullptr), last(last), next(next) {}
        node(const T& data) : last(nullptr), next(nullptr), data(new T(data)) {}
        node(const node& other) : last(nullptr), next(nullptr) {
            if (other.data != nullptr) {
                data = new T(*(other.data));
            } else
                data = nullptr;
        }

        ~node() {
            if (data != nullptr)
                delete data;
        }
    };
    struct block {
        size_t size;
        node *head,
            *tail;
        block *last,
            *next;
        block() : size(0), head(new node()), tail(new node()), last(nullptr), next(nullptr) {
            head->next = tail;
            tail->last = head;
        }
        block(const block& other) : head(new node()), tail(new node()), size(other.size), last(nullptr), next(nullptr) {
            node *nthis = head, *nother = other.head->next, *tmp;
            while (nother != other.tail) {
                tmp         = nthis;
                nthis->next = new node(*nother);
                nthis       = nthis->next;
                nthis->last = tmp;
                nother      = nother->next;
            }
            nthis->next = tail;
            tail->last  = nthis;
        }
        block(size_t size, node* head, node* tail, block* last, block* next) : size(size), head(head), tail(tail), last(last), next(next) {}
        ~block() {
            node* p = head;
            while (p != tail) {
                p = p->next;
                delete p->last;
            }
            delete tail;
        }
        //node and position
        node* no_at(size_t pos) const {
            node* p = head->next;
            while (pos) {
                p = p->next;
                pos--;
            }
            return p;
        }
        T& at(size_t pos) {
            return *(no_at(pos)->data);
        }
        size_t re_at(node* pos) const {
            node* p    = head->next;
            size_t loc = 0;
            while (p != pos) {
                ++loc;
                p = p->next;
            }
            return loc;
        }
        //int to resolve ambiguity
        void insert(const T& _data, node* at, int) {
            at             = at->last;
            node* ins      = new node(_data);
            ins->next      = at->next;
            at->next->last = ins;
            at->next       = ins;
            ins->last      = at;
            ++size;
            return;
        }
        void erase(node* at, int) {
            at       = at->last;
            at->next = at->next->next;
            delete at->next->last;
            at->next->last = at;
            --size;
        }
        void insert(const T& _data, const size_t pos) {
            return insert(_data, no_at(pos), 0);
        }
        void push_back(const T& _data) {
            return insert(_data, tail, 0);
        }
        void pop_back() {
            erase(tail->last, 0);
        }
        void push_front(const T& _data) {
            return insert(_data, 0);
        }
        void pop_front() {
            erase(head->next, 0);
        }

        T& front() const {
            return *(head->next->data);
        }
        T& back() const {
            return *(tail->last->data);
        }
        //Changed _! don't work if end
        //EMPTY_SAFE
        void split(node* pos) {
            node *left_tail, *right_head;
            left_tail        = new node();
            right_head       = new node();
            left_tail->last  = pos;
            right_head->next = pos->next;
            pos->next->last  = right_head;
            pos->next        = left_tail;
            block* ins       = new block(size - this->re_at(pos) - 1, right_head, tail, this, next);
            size             = this->re_at(pos) + 1;
            tail             = left_tail;
            next->last       = ins;
            next             = ins;
        }
        //UNSAFE
        void merge() {
            tail->last->next       = next->head->next;
            tail->last->next->last = tail->last;
            next->head->next       = next->tail;
            tail->last             = next->tail->last;
            tail->last->next       = tail;
            next->tail->last       = next->head;
            size += next->size;
            next = next->next;
            delete next->last;
            next->last = this;
        }
        bool exist(node* pos) const {
            node* rev = pos;
            for (int i = 0; i < (size >> 1) + 3; i++) {
                if (pos == tail || rev == head) {
                    return true;
                }
                if (pos == nullptr || rev == nullptr) {
                    return false;
                }
                pos = pos->next;
                rev = rev->last;
            }
            return false;
        }
    };

private:
    size_t _size;
    block *head, *tail;

    bool Collectable(size_t size) const { return size >= __CHUCKSIZE__ && (size * size >= __CHECKVALVE__ * _size); }
    block* bl_at(size_t& pos) const {
        block* p = head->next;
        while (pos >= p->size) {
            pos -= p->size;
            p = p->next;
        }
        return p;
    }

public:
    class const_iterator;
    class iterator {
        friend class deque;

    private:
        deque* deq;
        block* blk;
        node* pos;

    public:
        iterator() : deq(nullptr), blk(nullptr), pos(nullptr) {}
        iterator(const iterator& other) : deq(other.deq), blk(other.blk), pos(other.pos) {}
        iterator(deque* deq, block* blk, node* pos) : deq(deq), blk(blk), pos(pos) {}
        //Uhh. If problem I'll rewrite +-
        iterator& operator+=(const int& n) {
            if (n == 0)
                return *this;
            if (n < 0)
                return (*this -= (-n));
            size_t dist   = n + blk->re_at(pos);
            block* newblk = blk;
            while (dist >= newblk->size && newblk->next != deq->tail) {
                dist -= newblk->size;
                newblk = newblk->next;
            }
            node* newpos = newblk->head->next;
            while (dist) {
                --dist;
                newpos = newpos->next;
            }
            blk = newblk;
            pos = newpos;
            return *this;
        }
        iterator& operator-=(const int& n) {
            if (n == 0)
                return *this;
            if (n < 0)
                return (*this += (-n));
            int dist      = n - blk->re_at(pos);
            block* newblk = blk;
            while (dist > 0) {
                newblk = newblk->last;
                dist -= newblk->size;
            }
            dist         = -dist;
            node* newpos = newblk->head->next;
            while (dist) {
                --dist;
                newpos = newpos->next;
            }
            blk = newblk;
            pos = newpos;
            return *this;
        }
        iterator operator+(const int& n) const {
            return iterator(*this) += n;
        }
        iterator operator-(const int& n) const {
            return iterator(*this) -= n;
        }
        //DEST
        int operator-(const iterator& rhs) const {
            if (deq != rhs.deq) {
                throw invalid_iterator();
            }
            size_t index = blk->re_at(pos), rindex = rhs.blk->re_at(rhs.pos);
            block *lp = blk, *rp = rhs.blk, *tmp = deq->head->next;
            while (tmp != lp) {
                index += tmp->size;
                tmp = tmp->next;
            }
            tmp = rhs.deq->head->next;
            while (tmp != rp) {
                rindex += tmp->size;
                tmp = tmp->next;
            }
            return index - rindex;
        }
        //++iter
        iterator& operator++() {
            if (blk->next == deq->tail) {
                pos = pos->next;
            } else if (pos->next == blk->tail) {
                blk = blk->next;
                pos = blk->head->next;
            } else {
                pos = pos->next;
            }
            return *this;
        }
        //iter++
        iterator operator++(int) {
            iterator p(*this);
            ++(*this);
            return p;
        }
        //--iter
        iterator& operator--() {
            if (pos->last == blk->head) {
                blk = blk->last;
                pos = blk->tail->last;
            } else {
                pos = pos->last;
            }
            return *this;
        }
        //iter--
        iterator operator--(int) {
            iterator p(*this);
            --(*this);
            return p;
        }

        T& operator*() const {
            if (!blk->exist(pos) || blk->head == pos || blk->tail == pos) {
                throw invalid_iterator();
            }
            return *(pos->data);
        }
        T* operator->() const noexcept {
            return &*(*this);
        }

        bool operator==(const iterator& rhs) const {
            return ((deq == rhs.deq) && (blk == rhs.blk) && (pos == rhs.pos));
        }
        bool operator==(const const_iterator& rhs) const {
            return ((deq == rhs.deq) && (blk == rhs.blk) && (pos == rhs.pos));
        }
        bool operator!=(const iterator& rhs) const {
            return ((deq != rhs.deq) || (blk != rhs.blk) || (pos != rhs.pos));
        }
        bool operator!=(const const_iterator& rhs) const {
            return ((deq != rhs.deq) || (blk != rhs.blk) || (pos != rhs.pos));
        }
    };
    class const_iterator {

    private:
        const deque* deq;
        block* blk;
        node* pos;

    public:
        const_iterator() : deq(nullptr), blk(nullptr), pos(nullptr) {}
        const_iterator(const const_iterator& other) : deq(other.deq), blk(other.blk), pos(other.pos) {}
        const_iterator(const iterator& other) : deq(other.deq), blk(other.blk), pos(other.pos) {}
        const_iterator(const deque* deq, block* blk, node* pos) : deq(deq), blk(blk), pos(pos) {}
        //Uhh. If problem I'll rewrite +-
        const_iterator& operator+=(const int& n) {
            if (n == 0)
                return *this;
            if (n < 0)
                return (*this -= (-n));
            size_t dist   = n + blk->re_at(pos);
            block* newblk = blk;
            while (dist >= newblk->size && newblk->next != deq->tail) {
                dist -= newblk->size;
                newblk = newblk->next;
            }
            node* newpos = newblk->head->next;
            while (dist) {
                --dist;
                newpos = newpos->next;
            }
            blk = newblk;
            pos = newpos;
            return *this;
        }
        const_iterator& operator-=(const int& n) {
            if (n == 0)
                return *this;
            if (n < 0)
                return (*this += (-n));
            int dist      = n - blk->re_at(pos);
            block* newblk = blk;
            while (dist > 0) {
                newblk = newblk->last;
                dist -= newblk->size;
            }
            dist         = -dist;
            node* newpos = newblk->head->next;
            while (dist) {
                --dist;
                newpos = newpos->next;
            }
            blk = newblk;
            pos = newpos;
            return *this;
        }
        const_iterator operator+(const int& n) const {
            return const_iterator(*this) += n;
        }
        const_iterator operator-(const int& n) const {
            return const_iterator(*this) -= n;
        }
        //DIST
        int operator-(const const_iterator& rhs) const {
            if (deq != rhs.deq) {
                throw invalid_iterator();
            }
            size_t index = blk->re_at(pos), rindex = rhs.blk->re_at(rhs.pos);
            block *lp = blk, *rp = rhs.blk, *tmp = deq->head->next;
            while (tmp != lp) {
                index += tmp->size;
                tmp = tmp->next;
            }
            tmp = rhs.deq->head->next;
            while (tmp != rp) {
                rindex += tmp->size;
                tmp = tmp->next;
            }
            return index - rindex;
        }

        const_iterator& operator++() {
            if (blk->next == deq->tail) {
                pos = pos->next;
            } else if (pos->next == blk->tail) {
                blk = blk->next;
                pos = blk->head->next;
            } else {
                pos = pos->next;
            }
            return *this;
        }
        //????Why I use const_ it SEGSIGV?
        const_iterator operator++(int) {
            iterator p(deq, blk, pos);
            ++(*this);
            return p;
        }
        const_iterator& operator--() {
            if (pos->last == blk->head) {
                blk = blk->last;
                pos = blk->tail->last;
            } else {
                pos = pos->last;
            }
            return *this;
        }
        const_iterator operator--(int) {
            iterator p(deq, blk, pos);
            --(*this);
            return p;
        }
        T& operator*() const {
            if (!blk->exist(pos)) {
                throw invalid_iterator();
            }
            if (blk->head == pos || blk->tail == pos) {
                throw invalid_iterator();
            }
            return *(pos->data);
        }

        T* operator->() const noexcept {
            return &*(*this);
        }

        bool operator==(const iterator& rhs) const {
            return ((deq == rhs.deq) && (blk == rhs.blk) && (pos == rhs.pos));
        }
        bool operator==(const const_iterator& rhs) const {
            return ((deq == rhs.deq) && (blk == rhs.blk) && (pos == rhs.pos));
        }
        bool operator!=(const iterator& rhs) const {
            return ((deq != rhs.deq) || (blk != rhs.blk) || (pos != rhs.pos));
        }
        bool operator!=(const const_iterator& rhs) const {
            return ((deq != rhs.deq) || (blk != rhs.blk) || (pos != rhs.pos));
        }
    };

    deque() : _size(0), head(new block()), tail(new block()) {
        block* first = new block();
        head->next   = first;
        first->last  = head;
        first->next  = tail;
        tail->last   = first;
    }
    deque(const deque& other) : _size(other._size), head(new block()), tail(new block()) {
        block *nthis = head, *nother = other.head->next, *tmp;
        while (nother != other.tail) {
            tmp         = nthis;
            nthis->next = new block(*nother);
            nthis       = nthis->next;
            nthis->last = tmp;
            nother      = nother->next;
        }
        nthis->next = tail;
        tail->last  = nthis;
    }

    ~deque() {
        block* b = head;
        while (b != tail) {
            b = b->next;
            delete b->last;
        }
        delete tail;
    }
    deque& operator=(const deque& other) {
        if (&other == this)
            return *this;
        _size        = other._size;
        block *bthis = head->next, *bother = (other.head)->next, *tmp;
        while (bthis != tail) {
            tmp   = bthis;
            bthis = bthis->next;
            delete tmp;
        }
        bthis = head;
        while (bother != other.tail) {
            tmp         = bthis;
            bthis->next = new block(*bother);
            bthis       = bthis->next;
            bthis->last = tmp;
            bother      = bother->next;
        }
        bthis->next = tail;
        tail->last  = bthis;
        return *this;
    }
    T& at(const size_t& pos) {
        if (pos >= _size)
            throw index_out_of_bound();
        size_t ppos = pos;
        return bl_at(ppos)->at(ppos);
    }
    const T& at(const size_t& pos) const {
        if (pos >= _size)
            throw index_out_of_bound();
        size_t ppos = pos;
        return bl_at(ppos)->at(ppos);
    }
    T& operator[](const size_t& pos) {
        return at(pos);
    }
    const T& operator[](const size_t& pos) const {
        return at(pos);
    }

    const T& front() const {
        if (_size == 0)
            throw container_is_empty();
        return head->next->front();
    }
    const T& back() const {
        if (_size == 0)
            throw container_is_empty();
        return tail->last->back();
    }

    iterator begin() {
        return iterator(this, head->next, head->next->head->next);
    }
    const_iterator cbegin() const {
        return const_iterator(this, head->next, head->next->head->next);
    }
    iterator end() {
        return iterator(this, tail->last, tail->last->tail);
    }
    const_iterator cend() const {
        return const_iterator(this, tail->last, tail->last->tail);
    }
    bool empty() const { return !_size; }

    size_t size() const { return _size; }

    void clear() {
        _size    = 0;
        block* p = head->next;
        while (p != tail) {
            p = p->next;
            delete p->last;
        }
        head->next       = new block();
        head->next->last = head;
        head->next->next = tail;
        tail->last       = head->next;
    }

    iterator insert(iterator pos, const T& value) {
        if ((pos.deq != this) || !(pos.blk->exist(pos.pos))) {
            throw invalid_iterator();
        }
        pos.blk->insert(value, pos.pos, 0);
        _size++;
        pos = iterator(this, pos.blk, pos.pos->last);
        if (Collectable(pos.blk->size))
            pos.blk->split(pos.pos);
        return pos;
    }
    iterator erase(iterator pos) {
        if (_size == 0)
            throw container_is_empty();
        if ((pos.deq != this) || !(pos.blk->exist(pos.pos))) {
            throw invalid_iterator();
        }
        //lastent illegal iterator
        node* pnext = pos.pos->next;
        pos.blk->erase(pos.pos, 0);
        --_size;
        if (pnext == pos.blk->tail && pos.blk->next != pos.deq->tail) {
            if (pos.blk->next != pos.deq->tail) {
                pos.blk = pos.blk->next;
                pos.pos = pos.blk->head->next;
                return pos;
            } else {
                return end();
            }
        }
        pos.pos = pnext;
        //Here empty is processed.
        if ((pos.blk->next != tail) && (pos.blk->size + pos.blk->next->size <= __CHUCKSIZE__))
            pos.blk->merge();
        return pos;
    }

    void push_back(const T& value) {
        ++_size;
        block* p = tail->last;
        if (Collectable(tail->last->size)) {
            p->next       = new block();
            p->next->last = p;
            p->next->next = tail;
            tail->last    = p->next;
            p->next->push_back(value);
        } else {
            p->push_back(value);
        }
        return;
    }

    void pop_back() {
        if (_size == 0)
            throw container_is_empty();
        --_size;
        tail->last->pop_back();
        if (tail->last->size == 0 && tail->last->last != head) {
            block* del             = tail->last;
            tail->last->last->next = tail;
            tail->last             = tail->last->last;
            delete del;
        }
        return;
    }

    void push_front(const T& value) {
        ++_size;
        block* p = head->next;
        if (Collectable(p->size)) {
            head->next = new block();
            head->next->push_front(value);
            head->next->next       = p;
            head->next->last       = head;
            head->next->next->last = head->next;
        } else {
            p->push_front(value);
        }
    }

    void pop_front() {
        if (_size == 0)
            throw container_is_empty();
        --_size;
        head->next->pop_front();
        if (head->next->size == 0 && tail->last->last != head) {
            block* p         = head->next;
            head->next       = head->next->next;
            head->next->last = head;
            delete p;
            return;
        }
        if ((head->next->next != tail) && ((head->next->size + head->next->next->size) < __CHUCKSIZE__)) {
            head->next->merge();
        }
    }
};

} // namespace sjtu

#endif