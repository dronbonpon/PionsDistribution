#include <map>
#include <utility>

#include "CArrayWrapper.h"

template<typename Key, typename Pred>
void FillDistrMap( std::map<Key, int>& distributionMap_, std::size_t pointNumber, std::size_t upperBound, Pred pred, int param )
{
    for ( int i = 0; i < pointNumber; ++i )
    {
        int x = round( pred( param ) );

        if ( distributionMap_.find(x) != distributionMap_.end() )
        {
            distributionMap_[x]++;
        }
        else
        {
            distributionMap_[x] = 1;
        } 
    }

    for ( int i = 0; i < upperBound+1; i++ )
    {
        if ( distributionMap_.find(i) == distributionMap_.end() )
        {
            distributionMap_[i] = 0;
        }
    }
}
