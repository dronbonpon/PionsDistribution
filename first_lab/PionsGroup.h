#include <vector>
#include <cmath>

#include <TRandom.h>
#include <TRandom3.h>

#include "SinglePion.h"

struct PionsGroup
{
    std::size_t numberOfPions;
    std::vector<SinglePion> singlePions;

    PionsGroup() = default;
    PionsGroup( const PionsGroup& other )
    :numberOfPions( other.numberOfPions ), singlePions( other.singlePions )
    {}
    
    PionsGroup& operator=( const PionsGroup& other )
    {
        if ( this == &other )
        {
            return *this;
        }
        numberOfPions = other.numberOfPions;
        singlePions = other.singlePions;
        return *this;
    } 

    PionsGroup( int numberOfPions_ )
    : numberOfPions( numberOfPions_ )
    {
        std::unique_ptr<TRandom> rnd = std::make_unique<TRandom3>();
        singlePions = std::vector<SinglePion>( numberOfPions_ );
        for ( int i = 0; i < numberOfPions_; ++i )
        {
            double energy = rnd->Exp( 1000 );
            double absoluteRadius = std::sqrt( 2 * 139.5 * energy );
            double px, py, pz;
            rnd->Sphere( px, py, pz, absoluteRadius );
            singlePions[i] = SinglePion( energy, px, py, pz );
        }
    }
};