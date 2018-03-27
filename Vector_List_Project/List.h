//
// Created by h.shin on 2017-10-31.
//

#ifndef VECTORPROJECT_LIST_H
#define VECTORPROJECT_LIST_H

#include <bits/allocator.h>

template <typename Data, class Allocator = std::allocator<Data>>
class List
{
    struct node
    {
        Data data;
        node *prev;
        node *next;
        node(): data{0}, next{nullptr}, prev{nullptr}{};
    };

    typename Allocator::template rebind<node>::other ListAllocator;
    node *head;
    node *tail;
    size_t index;

public:
    class iterator : public std::iterator<std::bidirectional_iterator_tag, node>
    {
    private:
        node* current;

    public:
        iterator(node* current = nullptr)
        {
            this->current = current;
        }
        Data operator*() const
        {
            return this->current->data;
        }
        Data operator*()
        {
            return this->current->data;
        }
        operator node *()
        {
            return this->current;
        }
        operator node *() const
        {
            return this->current;
        }
        iterator& operator++()
        {
            this->current = this->current->next;
            return *this;
        }
        iterator operator+(size_t a)
        {
            while(a != 0)
            {
                this->current = this->current->next;
                a--;
                if(this->current == nullptr)
                    return nullptr;
            }
            return this->current;
        }

        iterator operator-(size_t b)
        {
            while(b != 0)
            {
                this->current = this->current->prev;
                b--;
                if(this->current == nullptr)
                    return nullptr;
            }
            return this->current;
        }
        const iterator operator++(int)
        {
            iterator copy(*this);
            this->current = this->current->next;
            return copy;
        }

        bool operator !=(const iterator& copy) const
        {
            bool flag = false;
            if(this->current != copy.current)
            {
                flag = true;
            }
            return flag;
        }

        bool operator ==(const iterator& copy) const
        {
            bool flag = false;
            if(this->current == copy.current)
            {
                flag = true;
            }
            return flag;
        }
    };

    typedef iterator const_iterator;

    List()
    {
        this->head = nullptr;
        this->tail = nullptr;
        this->index = 0;
    }

    List(const List& copy)
    {
        node* copyManager = copy.head;
        while(copyManager->next != nullptr)
        {
            this->push_back(copyManager->data);
            copyManager = copyManager->next;
        }
    }

    ~List()
    {   node* destroyer = nullptr;
        while(this->head != nullptr)
        {
            destroyer = this->head;
            this->head = this->head->next;
            this->ListAllocator.destroy(destroyer);
            this->ListAllocator.deallocate(destroyer, 1);
        }
    }

    void push_back(const Data &data)
    {
        node *newNode = this->ListAllocator.allocate(1);
        newNode->data = data;
        if(this->head == nullptr)
        {
            this->head = this->tail = newNode;
        }
        else
        {
            newNode->prev = this->tail;
            this->tail->next = newNode;
            newNode->next = nullptr;
            this->tail = newNode;
        }
        this->index++;
    }

    void remove(const Data &data)
    {
        node * remover = this->head;
        while(remover->data != data)
        {
            remover = remover->next;
        }
        remover->prev->next = remover->next;
        remover->next->prev = remover->prev;
        this->ListAllocator.deallocate(remover, 1);
        this->index--;
    }
    void hang_node(node *now,node *pos)
    {
        if(pos)
        {
            if(pos == head)
            {
                now->next = head;
                head->prev = now;
                head = now;
            }
            else
            {
                now->prev = pos->prev;
                now->next = pos;
                pos->prev->next = now;
                pos->prev = now;
            }
        }
        else
        {
            if(pos == head)
            {
                head = tail = now;
            }
            else
            {
                tail->next = now;
                now->prev = tail;
                tail = now;
            }
        }
    }

    void cutNode(node *pos)
    {
        if (pos->prev)
        {
            pos->prev->next = pos->next;
        }
        else
        {
           this->head = this->head->next;
        }
        if (pos->next)
        {
            pos->next->prev = pos->prev;
        }
        else
        {
            this->tail = this->tail->prev;
        }
    }

    iterator erase(iterator pos)
    {
        this->cutNode(pos);
        node* position = pos;
        this->ListAllocator.destroy(position);
        this->ListAllocator.deallocate(position, 1);
        this->index--;
    }

    void insert(iterator it, Data data)
    {
        node *current = this->ListAllocator.allocate(1);
        current->data = data;
        node *position = it;
        this->hang_node(current, position);
        this->index++;
    }
    iterator begin()
    {
        return this->head;
    }

    const_iterator begin() const
    {
        iterator copy(this->head);
        return copy;
    }
    iterator end()
    {
        return this->tail->next;
    }
    const_iterator end() const
    {
        iterator copy(this->tail->next);
        return copy;
    }
    size_t size()
    {
        return this->index;
    }

    bool empty()
    {
        bool flag = false;
        if(this->head == nullptr)
        {
            flag = true;
        }
        return flag;
    }

    size_t max_size()
    {
        if(this->head != nullptr)
        {
            if( sizeof(this->head->data) == sizeof(size_t))
            {
                return (pow(2, 30) - 1);
            }
            if( sizeof(this->head->data) == sizeof(double))
            {
                return (pow(2, 29) - 1);
            }
            if(sizeof(this->head->data) == sizeof(char))
            {
                return (pow(2, 32) - 1);
            }
        }
    }

    Data& front()
    {
        return this->head->data;
    }

    Data& back()
    {
        return this->tail->data;
    }

    void push_front(const Data& data)
    {
       this->insert(this->head, data);
    }

    void pop_front()
    {
        this->erase(this->head);
    }

    void pop_back()
    {
        this->erase(this->tail);
    }
    void resize(size_t size)
    {
        for(size_t n = this->index; n < size; n++)
        {
            this->push_back(0);
        }
        this->index = size;
    }
    void clear()
    {
        node* destroyer = nullptr;
        while(this->head != nullptr)
        {
            destroyer = this->head;
            this->head = this->head->next;
            this->ListAllocator.destroy(destroyer);
            this->ListAllocator.deallocate(destroyer, 1);
        }
        this->index = 0;
    }
    Allocator get_allocator()
    {
        return this->ListAllocator;
    }
};
#endif //VECTORPROJECT_LIST_H
