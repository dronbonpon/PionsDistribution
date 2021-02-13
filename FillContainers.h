#include <map>
#include <utility>

#include <TRandom3.h>
#include <TH1D.h>

#include "CArrayWrapper.h"

void FillDistrMap( std::map<int, int>& distributionMap_, std::size_t pointNumber, std::size_t upperBound )
{
    std::unique_ptr<TRandom3> rnd = std::make_unique<TRandom3>();
    rnd->SetSeed(1234);
    for ( int i = 0; i < pointNumber; ++i )
    {
        int x = round( rnd->Exp(4) );

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

void FillDistr( TH1D* distr, std::size_t pointNumber )
{
    std::unique_ptr<TRandom3> rnd = std::make_unique<TRandom3>();
    rnd->SetSeed(1234);
    for ( int i = 0; i < pointNumber; ++i )
    {
        int x = round( rnd->Exp(4) );

        distr->Fill(x);
    }
}