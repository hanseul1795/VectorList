//
// Created by h.shin on 2017-11-01.
//

#ifndef VECTORPROJECT_BASIC_STRING_H
#define VECTORPROJECT_BASIC_STRING_H

#include <bits/char_traits.h>
#include <bits/allocator.h>
#include <cstring>
#include <iostream>

template <typename Data, typename Trait = std::char_traits<Data>, class Allocator = std::allocator<Data> >
class string
{
private:
    static const size_t ArraySize = 15;
    Data* data;
    size_t maxLength;
    Data fixed[16];

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

    string(): data{nullptr}, maxLength{0} {};
    string(const Data* copy)
    {
       if(copy != nullptr)
       {
           this->maxLength = strlen(copy) + 1;
           if(strlen(copy) > 16)
           {
               this->data = Allocator().allocate(this->maxLength);
               for (size_t i = 0; i < this->maxLength; i++)
               {
                   Allocator().construct(this->data + i, copy[i]);
               }
           }
           else
           {
               memcpy(this->fixed, copy, strlen(copy) + 1);
               this->fixed[strlen(copy) + 1] = '\0';
           }
       }
    }

    string(const string& copy)
    {
        if(copy.data != nullptr && copy.maxLength - 1 > 16 && copy.maxLength - 1 != strlen(copy.data))
        {
            this->maxLength = strlen(copy.data) + 1;
            this->data = Allocator().allocate(this->maxLength);
            for (size_t i = 0; i < this->maxLength; i++)
            {
                Allocator().construct(this->data + i, copy.data[i]);
            }
        }
        else if(copy.data != nullptr && copy.maxLength - 1 > 16 && copy.maxLength - 1 == strlen(copy.data))
        {
            this->maxLength = copy.maxLength;
            this->data = Allocator().allocate(this->maxLength);
            for(size_t i = 0; i < this->maxLength; i++)
            {
                Allocator().construct(this->data + i, copy.data[i]);
            }
        }
        else if(copy.data != nullptr && copy.maxLength  <= 16)
        {
            this->maxLength = copy.maxLength;
            memcpy(this->fixed, copy.data, copy.maxLength);
        }
    }

    ~string()
    {
        if(this->data != nullptr && this->maxLength > 16)
        {
            for(size_t i = 0; i <= this->maxLength; i ++)
            {
                Allocator().destroy(this->data + i);
            }
            Allocator().deallocate(this->data, this->maxLength);
        }
    }

    size_t length()
    {
        if(this->maxLength == 0)
        {
            return 0;
        }
        else
        {
            return this->maxLength - 1;
        }
    }

    Data& operator[](size_t index)
    {
        if(index <= this->maxLength - 1)
        {
            return this->data[index];
        }
        else
        {
            throw "out of range";
        }
    }

    Data* c_str() const
    {
        return this->data;
    }
    string operator+(const string& copy)
    {
        if(this->fixed != nullptr && copy.fixed != nullptr && this->maxLength + copy.maxLength - 1 > 16)
        {
            size_t j = this->maxLength - 1;
            this->maxLength = ((this->ArraySize * 2) + 1);
            this->data = Allocator().allocate(this->maxLength);
            memcpy(this->data, this->fixed, j);
            for(size_t i = 0; i <= copy.maxLength; i++)
            {
                Allocator().construct(this->data + j, copy.fixed[i]);
                j++;
            }
        }
        if(this->fixed != nullptr && copy.fixed != nullptr && this->maxLength + copy.maxLength - 1 <= 16)
        {
            this->maxLength += (copy.maxLength -1);
            memcpy(this->fixed + this->maxLength, copy.fixed, copy.maxLength);
        }
        return *this;
    }

    string& operator=(const string& copy)
    {
        if(this->data != nullptr)
        {
            for(size_t i = 0; i <= this->maxLength; i ++)
            {
                Allocator().destroy(this->data + i);
            }
        }
        if(copy.data != nullptr)
        {
            if(this->maxLength == copy.maxLength)
            {
                for (size_t i = 0; i < this->maxLength; i++)
                {
                    Allocator().construct(this->data + i, copy.data[i]);
                }
            }
            else
            {
                this->maxLength = copy.maxLength;
                this->data = Allocator().allocate(this->maxLength);
                for (size_t i = 0; i < this->maxLength; i++)
                {
                    Allocator().construct(this->data + i, copy.data[i]);
                }
            }
        }
        return *this;
    }

    string& assign(size_t count, Data ch)
    {
        if(count > 16)
        {
            this->maxLength = count + 1;
            this->data = Allocator().allocate(count + 1);
            for (size_t i = 0; i < this->maxLength; i++)
            {
                Allocator().construct(this->data + i, ch);
            }
        }
        else
        {
            this->maxLength = count + 1;
            for (size_t i = 0; i < this->maxLength; i++)
            {
                Allocator().construct(this->fixed + i, ch);
            }
        }
        return *this;
    }

    Allocator get_allocator()
    {
        return Allocator();
    }

    Data& at(size_t pos)
    {
        if(pos > maxLength)
            throw "OUT OF RANGE";

        if(this->maxLength > 16)
        {
            return this->data[pos];
        }
        else
        {
            return this->fixed[pos];
        }
    }

    bool empty()
    {
        bool flag = false;
        if(this->data == nullptr || this->fixed == nullptr)
        {
            flag = false;
        }
        else
        {
            flag = true;
        }
        return flag;
    }

    Data& front()
    {
        if(this->data != nullptr)
        {
            return this->data[0];
        }
        else if(this->fixed != nullptr)
        {
            return this->fixed[0];
        }
        else
        {
            throw "Container is empty";
        }
    }

    Data& back()
    {
        if(this->data != nullptr)
        {
            return this->data[strlen(this->data)];
        }
        else if(this->fixed != nullptr)
        {
            return this->fixed[strlen(this->fixed)];
        }
        else
        {
            throw "Container is empty";
        }
    }

    Data* get_data()
    {
        if(this->data == nullptr)
        {
            return nullptr;
        }
        else
        {
            return this->data;
        }
    }

    iterator begin()
    {
        if(this->data == nullptr && this->fixed != nullptr)
        {
            return this->fixed;
        }
        if(this->data != nullptr)
        {
            return this->data;
        }
        else
        {
            return nullptr;
        }
    }

    iterator end()
    {
        if(this->data == nullptr && this->fixed != nullptr)
        {
            return this->fixed + (strlen(this->fixed) + 1);
        }
        if(this->data != nullptr)
        {
            return this->data + (strlen(this->data) + 1);
        }

        else
        {
            return nullptr;
        }
    }

    size_t capacity() const
    {
        return this->maxLength - 1;
    }

    void clear()
    {
        if(this->data != nullptr && this->maxLength > 16)
        {
            for(size_t i = 0; i <= this->maxLength; i ++)
            {
                Allocator().destroy(this->data + i);
            }
            Allocator().deallocate(this->data, this->maxLength);
        }
    }
};
#endif //VECTORPROJECT_BASIC_STRING_H
