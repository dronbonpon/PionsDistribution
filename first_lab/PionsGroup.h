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

    template<typename EnergyPred, typename MomentumPred>
    PionsGroup( int numberOfPions_, EnergyPred energyPred, int energyParam, MomentumPred momentumPred, double pionMass )
    : numberOfPions( numberOfPions_ )
    {
        std::unique_ptr<TRandom> rnd = std::make_unique<TRandom3>();
        singlePions = std::vector<SinglePion>( numberOfPions_ );
        for ( int i = 0; i < numberOfPions_; ++i )
        {
            double energy = energyPred( energyParam );
            std::cout << energy << std::endl;
            double absoluteRadius = std::sqrt( 2 * pionMass * energy );
            double px, py, pz;
            momentumPred( px, py, pz, absoluteRadius );
            std::cout << px << " " << py << " " << pz << std::endl;
            singlePions[i] = SinglePion( energy, px, py, pz );
        }
    }
};