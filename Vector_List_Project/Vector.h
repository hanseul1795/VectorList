//
// Created by h.shin on 2017-10-31.
//

#ifndef VECTORPROJECT_VECTOR_H
#define VECTORPROJECT_VECTOR_H

#include <bits/allocator.h>
#include <cmath>

template <typename Data, class Allocation = std::allocator<Data>>
class vector
{
private:
    Allocation allocator;
    Data *Storage;
    unsigned long int StoredItem;
    unsigned long int StorageCapacity;

public:
    class iterator //: std::iterator<std::random_access_iterator_tag, Data>
    {
    private:
        Data* position;

    public:
        iterator(Data* position = nullptr)
        {
            this->position = position;
        }

        Data operator*()
        {
            return *this->position;
        }

        size_t operator-(const iterator &other)
        {
            return this->position - other.position;
        }

        iterator operator+(size_t a)
        {
            while(a != 0)
            {
                this->position++;
                a--;
            }
            return this->position;
        }

        iterator operator-(size_t b)
        {
            while(b != 0)
            {
                this->position--;
                b--;
            }
            return this->position;
        }

        iterator& operator++()
        {
            this->position++;
            return *this;
        }

        const iterator operator++(int)
        {
            iterator at(*this);
            this->position++;
            return at;
        }

        bool operator !=(const iterator &other)
        {
            bool flag = false;
            if(this->position != other.position)
            {
                flag = true;
            }
            return flag;
        }

        bool operator ==(const iterator &other)
        {
            bool flag = false;
            if(this->position == other.position)
            {
                flag = true;
            }
            return flag;
        }
    };

    typedef const iterator const_iterator;

    class reverse_iterator //: std::iterator<std::random_access_iterator_tag, Data>
    {
    private:
        Data* position;

    public:
        reverse_iterator(Data* position = nullptr)
        {
            this->position = position;
        }

        Data operator*()
        {
            return *this->position;
        }

        size_t operator-(const reverse_iterator &other)
        {
            return this->position - other.position;
        }

        reverse_iterator operator+(size_t a)
        {
            while(a != 0)
            {
                this->position--;
                a--;
            }
            return this->position;
        }

        reverse_iterator operator-(size_t b)
        {
            while(b != 0)
            {
                this->position++;
                b--;
            }
            return this->position;
        }

        reverse_iterator& operator++()
        {
            this->position--;
            return *this;
        }

        const reverse_iterator operator++(int)
        {
            reverse_iterator at(*this);
            this->position--;
            return at;
        }

        bool operator !=(const reverse_iterator &other)
        {
            bool flag = false;
            if(this->position != other.position)
            {
                flag = true;
            }
            return flag;
        }

        bool operator ==(const reverse_iterator &other)
        {
            bool flag = false;
            if(this->position == other.position)
            {
                flag = true;
            }
            return flag;
        }
    };

    typedef const reverse_iterator const_reverse_iterator;

    vector(): Storage{nullptr}, StoredItem{0}, StorageCapacity{0} {};

    vector(size_t a)
    {
        this->Storage = this->allocator.allocate(a);
        this->StorageCapacity = a;
        for(size_t i = 0; i < a ; i ++)
        {
            this->allocator.construct(this->Storage + i , 0);
        }
        this->StoredItem = a;
    }
    vector(const vector &copy)
    {
        this->Storage = nullptr;
        this->StorageCapacity = 0;
        this->StoredItem = 0;
        this->reserve(copy.StorageCapacity);
        for (size_t i = 0; i < copy.StoredItem; i++)
        {
            this->allocator.construct(this->Storage + i, copy.Storage[i]);
        }
        this->StorageCapacity = copy.StorageCapacity;
        this->StoredItem = copy.StoredItem;
    }

    ~vector()
    {
        if (this->StoredItem != 0)
        {
            for (size_t i = 0; i < this->StoredItem; i++)
            {
                this->allocator.destroy(this->Storage + i);
            }
            this->allocator.deallocate(this->Storage, this->StorageCapacity);
        }
    }

    vector &operator=(const vector &copy)
    {
        this->reserve(copy.StorageCapacity);
        for (size_t i = 0; i < copy.StoredItem; i++)
        {
            this->allocator.construct(this->Storage + i, copy.Storage[i]);
        }
        this->StorageCapacity = copy.StorageCapacity;
        this->StoredItem = copy.StoredItem;
        return *this;
    }

    unsigned long int capacity() const
    {
        return this->StorageCapacity;
    }

    unsigned long int size() const
    {
        return this->StoredItem;
    }

    void clear()
    {
        size_t i;
        for (i = 0; i < this->StoredItem; i++) {
            this->allocator.destroy(this->Storage + i);
        }
        this->StoredItem -= i;
    }

    void reserve(size_t capacity) {
        if (capacity < this->StorageCapacity)
            return;

        else {
            Data *temp = this->allocator.allocate(capacity);
            if (this->StoredItem != 0) {
                for (size_t i = 0; i < this->StoredItem; i++)
                {
                    this->allocator.construct(temp + i, this->Storage[i]);
                }
                for (size_t i = 0; i < this->StoredItem; i++)
                    this->allocator.destroy(this->Storage + i);

                this->allocator.deallocate(this->Storage, this->StorageCapacity);
            }
            this->StorageCapacity = capacity;
            this->Storage = temp;
        }
    }

    void push_back(const Data& data) {
        size_t newCapacity = 0;
        if (this->StoredItem == this->StorageCapacity)
        {
            if (this->StorageCapacity == 0)
            {
                newCapacity = 1;
            }
            else
            {
                newCapacity = 2* this->StoredItem;
            }
            Data *temp = this->allocator.allocate(newCapacity);
            this->allocator.construct(temp + this->StoredItem, data);
            if (this->StorageCapacity > 0)
            {
                for (size_t i = 0; i < this->StoredItem; i++)
                {
                    this->allocator.construct(temp + i, this->Storage[i]);
                }
                for (size_t i = 0; i < this->StoredItem; i++)
                {
                    this->allocator.destroy(this->Storage + i);
                }
                this->allocator.deallocate(this->Storage, this->StorageCapacity);
            }
            ++this->StoredItem;
            this->Storage = temp;
            this->StorageCapacity = newCapacity;
        }
        else
        {
            this->allocator.construct(this->Storage + this->StoredItem, data);
            ++this->StoredItem;
        }
    }

    void pop_back()
    {
        if(this->Storage != nullptr)
        {
            this->allocator.destroy(this->Storage + (this->StoredItem - 1));
            this->StoredItem--;
        }
    }

    iterator insert(iterator pos, const Data &data)
    {
        size_t index = pos - this->Storage;
        if(this->StoredItem == this->StorageCapacity)
        {
            Data *temp = this->allocator.allocate(this->StorageCapacity + 1);
            if (this->StoredItem != 0)
            {
                for (size_t i = 0; i < this->StoredItem; i++)
                {
                    this->allocator.construct(temp + i, this->Storage[i]);
                }
                for (size_t i = 0; i < this->StoredItem; i++)
                    this->allocator.destroy(this->Storage + i);

                this->allocator.deallocate(this->Storage, this->StorageCapacity);
            }
            this->StorageCapacity += 1;
            this->Storage = temp;
        }
        for(size_t n = this->StoredItem; n > index; n--)
        {
            this->Storage[n] = this->Storage[n-1];
        }
        this->Storage[index] = data;
        this->StoredItem++;
        return pos;
    }

    reverse_iterator insert(reverse_iterator pos, const Data &data)
    {
        size_t index = pos - this->Storage;
        if(this->StoredItem == this->StorageCapacity)
        {
            Data *temp = this->allocator.allocate(this->StorageCapacity + 1);
            if (this->StoredItem != 0)
            {
                for (size_t i = 0; i < this->StoredItem; i++)
                {
                    this->allocator.construct(temp + i, this->Storage[i]);
                }
                for (size_t i = 0; i < this->StoredItem; i++)
                    this->allocator.destroy(this->Storage + i);

                this->allocator.deallocate(this->Storage, this->StorageCapacity);
            }
            this->StorageCapacity += 1;
            this->Storage = temp;
        }
        for(size_t n = this->StoredItem; n > index; n--)
        {
            this->Storage[n] = this->Storage[n-1];
        }
        this->Storage[index] = data;
        this->StoredItem++;
        return pos;
    }

    iterator erase(iterator it)
    {
        size_t index = it - this->Storage;
        for(size_t n = index + 1; n < this->StoredItem; n++)
        {
            this->Storage[n - 1] = this->Storage[n];
        }
        this->allocator.destroy(this->Storage + index);
        this->StoredItem--;
        return it;
    }

    reverse_iterator erase(reverse_iterator it)
    {
        size_t index = it - this->Storage;
        this->allocator.destroy(this->Storage + index);
        for(size_t n = index + 1; n < this->StoredItem; n++)
        {
            this->Storage[n - 1] = this->Storage[n];
        }
        this->StoredItem--;
        return it;
    }

    void resize(const size_t reSize)
    {
        if (reSize > this->StorageCapacity)
        {
            this->reserve(reSize);
            for (size_t i = this->StoredItem; i < reSize; i++)
            {
                this->allocator.construct(this->Storage + i);
            }
            this->StoredItem += (reSize - this->StoredItem);
        }
        if(reSize < this->StorageCapacity && reSize > this->StoredItem)
        {
            for (size_t i = this->StoredItem; i < reSize; i++)
            {
                this->allocator.construct(this->Storage + i);
            }
            this->StoredItem += (reSize - this->StoredItem);
        }
        if(reSize < this->StoredItem)
        {
            Data *temp = this->allocator.allocate(reSize);
            if (this->StoredItem != 0)
            {
                for (size_t i = 0; i < reSize; i++)
                {
                    this->allocator.construct(temp + i, this->Storage[i]);
                }
                for (size_t i = 0; i < this->StoredItem; i++)
                {
                    this->allocator.destroy(this->Storage + i);
                }
                this->allocator.deallocate(this->Storage, this->StorageCapacity);
            }
            this->StoredItem = reSize;
            this->StorageCapacity = reSize;
            this->Storage = temp;
        }
    }

    bool empty() const
    {
        bool flag = false;
        if(this->StoredItem == 0)
        {
            flag = true;
        }
        return flag;
    }

    void shrink_to_fit()
    {
        this->StorageCapacity = this->StoredItem;
    }

    void swap(vector& toSwap)
    {
        if(this->StorageCapacity > toSwap.StorageCapacity)
        {
            toSwap.resize(this->StorageCapacity);
            Data * temp = this->allocator.allocate(this->StorageCapacity);
            for (size_t i = 0; i < this->StoredItem ; i++)
            {
                this->allocator.construct(temp + i , this->Storage[i]);
            }
            for (size_t i = 0; i < this->StoredItem; i++)
            {
                this->allocator.destroy(this->Storage + i);
            }
            for (size_t i = 0; i < toSwap.StoredItem ; i++)
            {
                this->allocator.construct(this->Storage + i , toSwap.Storage[i]);
            }
            for (size_t i = 0; i < this->StoredItem ; i++)
            {
                this->allocator.construct(toSwap.Storage + i, temp[i]);
            }
            this->allocator.deallocate(temp, this->StorageCapacity);
        }
        if(this->StorageCapacity == toSwap.StorageCapacity)
        {
            Data * temp = this->allocator.allocate(this->StorageCapacity);
            for (size_t i = 0; i < this->StoredItem ; i++)
            {
                this->allocator.construct(temp + i , this->Storage[i]);
            }
            for (size_t i = 0; i < this->StoredItem; i++)
            {
                this->allocator.destroy(this->Storage + i);
            }
            for (size_t i = 0; i < toSwap.StoredItem ; i++)
            {
                this->allocator.construct(this->Storage + i , toSwap.Storage[i]);
            }
            for (size_t i = 0; i < this->StoredItem ; i++)
            {
                this->allocator.construct(toSwap.Storage + i, temp[i]);
            }
            this->allocator.deallocate(temp, this->StorageCapacity);
        }
        else
        {
            this->resize(toSwap.StorageCapacity);
            Data * temp = this->allocator.allocate(this->StorageCapacity);
            for (size_t i = 0; i < this->StoredItem ; i++)
            {
                this->allocator.construct(temp + i , this->Storage[i]);
            }
            for (size_t i = 0; i < this->StoredItem; i++)
            {
                this->allocator.destroy(this->Storage + i);
            }
            for (size_t i = 0; i < toSwap.StoredItem ; i++)
            {
                this->allocator.construct(this->Storage + i , toSwap.Storage[i]);
            }
            for (size_t i = 0; i < this->StoredItem ; i++)
            {
                this->allocator.construct(toSwap.Storage + i, temp[i]);
            }
            this->allocator.deallocate(temp, this->StorageCapacity);
        }
    }
    Data &operator[](size_t index)
    {
        if (index < this->StoredItem)
        {
            return this->Storage[index];
        }
        else
        {
            throw "Used number is too big";
        }
    }

    Data& at(size_t index)
    {
        if (index < this->StoredItem)
        {
            return this->Storage[index];
        }
        else
        {
            throw "Used number is too big";
        }
    }

    Data& front()
    {
        return this->Storage[0];
    }

    Data& back()
    {
        return this->Storage[this->StoredItem - 1];
    }

    iterator begin()
    {
        return this->Storage;
    }

    iterator end()
    {
        return this->Storage + this->StoredItem + 1;
    }

    reverse_iterator rbegin()
    {
        return this->Storage + this->StoredItem;
    }

    reverse_iterator rend()
    {
        return this->Storage;
    }

    const_iterator cbegin() const
    {
        return this->Storage;
    }

    const_iterator cend() const
    {
        return this->Storage + this->StoredItem;
    }

    const_reverse_iterator crbegin() const
    {
        return this->Storage + this->StoredItem;
    }

    const_reverse_iterator crend() const
    {
        return this->Storage;
    }

    Allocation get_allocator() const
    {
        return this->allocator;
    }
    void assign(size_t n, const Data& data)
    {
        if(this->StorageCapacity < n)
        {
            this->resize(n);
            for(size_t i = 0; i <this->StoredItem; i++)
            {
                this->allocator.destroy(this->Storage + i);
            }
            for(size_t i = 0; i < n; i++)
            {
                this->allocator.construct(this->Storage + i, data);
            }
        }
        else
        {
            for(size_t i = 0; i <this->StoredItem; i++)
            {
                this->allocator.destroy(this->Storage + i);
            }
            for(size_t i = 0; i < n ; i++)
            {
                this->allocator.construct(this->Storage + i, data);
            }
        }
    }

    Data* data()
    {
        return this->Storage;
    }

    size_t max_size()
    {
        if(this->Storage != nullptr)
        {
            if( sizeof(this->front()) == sizeof(size_t))
            {
                return (pow(2, 30) - 1);
            }
            if( sizeof(this->front()) == sizeof(double))
            {
                return (pow(2, 29) - 1);
            }
            if(sizeof(this->front()) == sizeof(char))
            {
                return (pow(2, 32) - 1);
            }
        }
    }
};

#endif //VECTORPROJECT_VECTOR_H
