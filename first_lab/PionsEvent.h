#include <vector>
#include <cmath>

#include "SinglePion.h"

struct PionsEvent
{
    std::size_t numberOfPions;
    std::vector<SinglePion> singlePions;

    PionsEvent() = default;
    PionsEvent( const PionsEvent& other )
    :numberOfPions( other.numberOfPions ), singlePions( other.singlePions )
    {}
    
    PionsEvent& operator=( const PionsEvent& other )
    {
        if ( this == &other )
        {
            return *this;
        }
        numberOfPions = other.numberOfPions;
        singlePions = other.singlePions;
        return *this;
    } 

    /// Конструктор класса PionsEvent
    /// numberOfPions_ - число пионов в событии
    /// energyPred - функция, которая определяет значение энергии для пионов в данном событии
    /// energyParam - параметр этой функции
    /// momentumPred - функция, которая определяет значения трех проекций импульса
    /// pionMass - масса пионов в событии
    template<typename EnergyPred, typename MomentumPred, typename EnergyParam >
    PionsEvent( int numberOfPions_, EnergyPred energyPred, EnergyParam energyParam, 
                MomentumPred momentumPred, double pionMass )
    : numberOfPions( numberOfPions_ )
    {
        std::unique_ptr<TRandom> rnd = std::make_unique<TRandom3>();
        singlePions = std::vector<SinglePion>( numberOfPions_ );
        for ( int i = 0; i < numberOfPions_; ++i )
        {
            double energy = energyPred( energyParam );
            double absoluteRadius = std::sqrt( 2 * pionMass * energy );
            double px, py, pz;
            momentumPred( px, py, pz, absoluteRadius );
            singlePions[i] = SinglePion( energy, px, py, pz );
        }
    }
};