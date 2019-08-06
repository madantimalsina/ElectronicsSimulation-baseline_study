//
//  Buffer.hpp
//  Buffer
//
//  Created by Cees Carels on 25/04/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef Buffer_hpp
#define Buffer_hpp

#include <iostream>
#include <stdio.h>
#include <vector>

template <class TYPE>
class Buffer
{
public:
    void init(const int size);
    TYPE val();
    TYPE at(const int i);
    int head();
    void resize();
    void flush();
    void add(const TYPE val);
    unsigned long distance(const int fst, const int snd);

protected:
    unsigned long SIZE;

    std::vector<TYPE> buf;
    int pos1;
    int pos2;
};

template <class TYPE>
void Buffer<TYPE>::init(const int size)
{
    pos1 = 0; //Head for writing
    pos2 = 0; //Tail for reading
    SIZE = size;
    buf.resize(SIZE);
}

template <class TYPE>
TYPE Buffer<TYPE>::val()
{
    if (pos2 == SIZE)
        pos2 = 0;
    return buf[pos2++];
}

template <class TYPE>
TYPE Buffer<TYPE>::at(const int i)
{
    //std::cout << "i " << i << " buf-idx " << i%SIZE << std::endl;
    return buf[i % SIZE];
}

template <class TYPE>
int Buffer<TYPE>::head()
{
    return pos1;
}

template <class TYPE>
void Buffer<TYPE>::resize()
{
}

template <class TYPE>
void Buffer<TYPE>::flush()
{
    for (int i = 0; i < buf.size(); i++)
    {
        buf[i] = 0;
    }
}

template <class TYPE>
void Buffer<TYPE>::add(const TYPE val)
{
    if (pos1 == SIZE)
        pos1 = 0;
    buf[pos1++] = val;
    //std::cout << "Added " << val << " at " << pos1 << std::endl;
}

template <class TYPE>
unsigned long Buffer<TYPE>::distance(const int fst,
    const int snd)
{
    int a = (fst - snd) % SIZE;
    int b = (snd - fst) % SIZE;
    //return (a == b) ? SIZE : ((a < b) ? a : b);//Return full distance
    return (a < b) ? a : b;
}

#endif /* Buffer_hpp */
