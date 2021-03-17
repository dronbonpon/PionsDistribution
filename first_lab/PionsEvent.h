#pragma once

#include <vector>
#include <cmath>
#include <thread>

#include "SingleParticle.h"
#include "threadRAII.h"


/// Конструктор класса PionsEventParams
/// numberOfPions_ - число пионов в событии
/// pionMass - масса пионов в событии
/// energyPred - функция, которая определяет значение энергии для пионов в данном событии
/// energyParam - параметр этой функции
/// momentumPred - функция, которая определяет значения трех проекций импульса
template<typename EnergyPred, typename MomentumPred, typename EnergyParam>
struct PionsEventParams
{

friend struct PionsEvent;

    PionsEventParams( int numberOfPions, double pionMass, EnergyPred energyPred,
                      EnergyParam energyParam, MomentumPred momentumPred )
    : numberOfPions_( numberOfPions ),
      pionMass_( pionMass ),
      energyPred_( energyPred ),
      energyParam_( energyParam ),
      momentumPred_( momentumPred )
    {}  

private:
    int numberOfPions_;
    double pionMass_;
    EnergyPred energyPred_;
    EnergyParam energyParam_;
    MomentumPred momentumPred_;
};

template<typename EnergyPred, typename MomentumPred, typename EnergyParam>
PionsEventParams<EnergyPred, MomentumPred, EnergyParam> 
createPionsEventParams( int numberOfPions, double pionMass, EnergyPred energyPred,
                                         EnergyParam energyParam, MomentumPred momentumPred )
{
    return PionsEventParams<EnergyPred, MomentumPred, EnergyParam>( 
           numberOfPions, pionMass, energyPred, energyParam, momentumPred
     );
}

struct MomentumVector
{

friend struct PionsEvent;

    MomentumVector( std::vector<float>& px,
                    std::vector<float>& py,
                    std::vector<float>& pz,
                    std::vector<float>& ip)
    : px_( std::move(px) ),
      py_( std::move(py) ),
      pz_( std::move(pz) ),
      ip_( std::move(ip) )
    {}      

private:
    const std::vector<float> px_;
    const std::vector<float> py_;
    const std::vector<float> pz_;
    const std::vector<float> ip_;
};

struct PionsEvent
{
    std::size_t numberOfPions;
    std::vector<SingleParticle> singleParticles;

    PionsEvent() = default;
    PionsEvent( const PionsEvent& other ) = default;
    PionsEvent& operator=( const PionsEvent& other ) = default;

    template<typename EnergyPred, typename MomentumPred, typename EnergyParam>
    PionsEvent( PionsEventParams<EnergyPred, MomentumPred, EnergyParam> pionsEventParams )
    : numberOfPions( pionsEventParams.numberOfPions_ )
    {
        std::unique_ptr<TRandom> rnd = std::make_unique<TRandom3>();
        singleParticles = std::vector<SingleParticle>( numberOfPions );
        for ( int i = 0; i < numberOfPions; ++i )
        {
            if ( pionsEventParams.pionMass_ <= 0 )
            {
                throw std::runtime_error( "Pion's mass must be positive" );
            }
            
            double energy = pionsEventParams.energyPred_( pionsEventParams.energyParam_ );
            double absoluteRadius = std::sqrt( 2 * pionsEventParams.pionMass_ * energy );
            double px, py, pz;
            pionsEventParams.momentumPred_( px, py, pz, absoluteRadius );
            singleParticles[i] = SingleParticle( energy, px, py, pz );
        }
    }

    PionsEvent( const MomentumVector& momentumVector, int numberOfPions_ )
    : numberOfPions( numberOfPions_ )
    {
        for ( int i = 0; i < numberOfPions; ++i )
        {
            SingleParticle pion( momentumVector.ip_[i],
                                 momentumVector.px_[i], 
                                 momentumVector.py_[i],
                                 momentumVector.pz_[i] );
            singleParticles.push_back( pion );
        }
    }
};