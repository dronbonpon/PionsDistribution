#include <stdexcept>
#include <ostream>
#include <iostream>
#include <map>


template<typename Number>
class CArrayWrapper
{
    Number* data;
    std::size_t size;

public:
    
    CArrayWrapper( std::size_t size_ )
    : size(size_)
    {
        data  = new Number[size_];
    }

    Number* ToArray()
    {
        return data;
    }

    Number& operator[](int i)
    {
        if ( i < 0 )
        {
            throw std::runtime_error( "Negative index" );
        }
        if ( i >= size )
        {
            throw std::runtime_error( "Out of index" );
        }
        return data[i];
    }

    Number operator[](int i) const
    {
        return this->operator[](i);
    }

    std::size_t Size() const
    {
        return size;
    }
    
    ~CArrayWrapper()
    {
        delete[] data;
    }
};

template<typename Number>
void FillXAxis( CArrayWrapper<Number>& xAxis, std::size_t upperBound )
{
    for ( int i = 0; i < upperBound+1; i++ )
    {
        xAxis[i] = i;
    }
}

template<typename Key, typename Number>
void FillYAxis( CArrayWrapper<Number>& yAxis, std::map<Key, int>& distributionMap_ )
{
    int i = 0;
    for ( const auto & pair : distributionMap_ )
    {
        if ( i < yAxis.Size() )
        {
            yAxis[i] = abs( pair.second );
            i++;    
        }
        else
        {
            break;
        }
    }
}