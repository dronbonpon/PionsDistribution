#pragma once

#include <vector>
#include <cmath>
#include <thread>

#include "SinglePion.h"
#include "thread_RAII.h"

struct PionsEvent
{
    std::size_t numberOfPions;
    std::vector<SinglePion> singlePions;

    PionsEvent() = default;
    PionsEvent( const PionsEvent& other ) = default;
    PionsEvent& operator=( const PionsEvent& other ) = default; 

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
            if ( pionMass <= 0 )
            {
                throw std::runtime_error( "Pion's mass must be positive" );
            }
            
            double energy = energyPred( energyParam );
            double absoluteRadius = std::sqrt( 2 * pionMass * energy );
            double px, py, pz;
            momentumPred( px, py, pz, absoluteRadius );
            singlePions[i] = SinglePion( energy, px, py, pz );
        }
    }

    PionsEvent( const std::vector<float>& px, const std::vector<float>& py, 
                const std::vector<float>& pz, const std::vector<float>& ip, int numberOfPions_ )
    : numberOfPions( numberOfPions_ )
    {
        for ( int i = 0; i < numberOfPions; ++i )
        {
            SinglePion pion( ip[i], px[i], py[i], pz[i] );
            singlePions.push_back( pion );
        }
    }
};