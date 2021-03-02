#pragma once

#include <vector>

#include <TRandom.h>
#include <TRandom3.h>

#include "thread_RAII.h"
#include "PionsEvent.h"

#ifdef __linux__
const static int TASK_NUMBER = sysconf( _SC_NPROCESSORS_ONLN ) / 2;
#else
const static int TASK_NUMBER = 4;
#endif

void ComputeSequentially( std::vector<PionsEvent>& pions,
                          int startIndex, int endIndex )
{
    
    std::unique_ptr<TRandom> rnd = std::make_unique<TRandom3>();
    
    for ( int i = startIndex; i < endIndex; ++i )
    {
        int numberOfPions = round( rnd->Exp( 4 ) );

        auto energyPred = [&rnd]( int energyParam )
        {   
            return rnd->Exp( energyParam );
        };
        int energyParam = 1000; // В МЭв

        auto momentumPred = [&rnd]( double& px, double& py, double& pz, double absoluteRadius )
        {
            rnd->Sphere( px, py, pz, absoluteRadius );
        };

        double pionMass = 139.5;

        // Заполняем значения события, количество частиц в котором равно 
        // случайному числу, распределенному по экспоненциальному закону с параметром energyParam
        // Импульс и энергия каждой частицы в каждом событии также выбирается случайно:
        // Энергия - по экспоненциальному закону с параметром 1000 (в МЭв),
        // Импульс - по равномерно распределенной сфере с радиусом sqrt( 2 * масса пионов * величина энергии )
        pions[i] = PionsEvent( numberOfPions, energyPred, energyParam, momentumPred, pionMass );
    }
}

void ComputeParallel( std::vector<PionsEvent>& pions )
{
    int numberOfTasks;

    TASK_NUMBER ? numberOfTasks = TASK_NUMBER : numberOfTasks = 1;

    std::cout << "Parrallel computing on " << numberOfTasks << " threads" << std::endl;
    
    std::vector<ThreadRAII> threads;

    int sizeOfSequentialTask = ( pions.size() % 6 == 0 ) ? 
                                pions.size()/numberOfTasks : pions.size()/numberOfTasks + 1;

    int i = sizeOfSequentialTask;
    
    while ( i < pions.size() )
    {
        ThreadRAII currentThread( std::thread( ComputeSequentially, std::ref( pions ), i-sizeOfSequentialTask, i ), 
                                  ThreadRAII::DtorAction::join );
        threads.emplace_back( std::move( currentThread ) );
        i += sizeOfSequentialTask;
    }

    i -= sizeOfSequentialTask;
    ThreadRAII currentThread( std::thread( ComputeSequentially, std::ref( pions ), i, pions.size() ), 
                              ThreadRAII::DtorAction::join );
    threads.emplace_back( std::move( currentThread ) );

}

// Общая функция для заполнения вектора из событий пионов
// В зависимости от размера вектора вызызвается либо функция,
// которая выполняет вычисления последовательно, либо та, которая 
// делает это параллельно
void Compute( std::vector<PionsEvent>& pions )
{
    if ( pions.size() > 100000 )
    {
        return ComputeParallel( pions );
    }
    return ComputeSequentially( pions, 0, pions.size() );
}

// Считает быстроту, псевдобыстроту и азимутальный угол вектора
// из событий пионов для каждого отдельного пиона
void ComputeParams( std::vector<PionsEvent>& pions )
{
    for ( auto & event : pions )
    {
        for ( auto & pion : event.singlePions )
        {
            double absMomentum = std::sqrt( pion.px*pion.px + pion.py*pion.py + pion.pz*pion.pz );
            double rapidity = 0.5 * std::log( ( pion.energy + pion.pz ) /
                                                ( pion.energy - pion.pz ) );
            pion.SetRapidity( rapidity );
            double pseudorapidity = 0.5 * std::log( ( absMomentum + pion.pz )/( absMomentum - pion.pz ) );
            pion.SetPseudorapidity( pseudorapidity );
            if ( pion.px != 0 )
            {
                double azimuthalAngle = std::atan( pion.py / pion.px );
                pion.SetAzimuthalAngle( azimuthalAngle );
            }
        }
    }
}

// Выполняет преобразование Лоренца для вектора из событий пионов
void LorentzTransformation( std::vector<PionsEvent>& pions, double b )
{
    int speedOfLight = 1;
    for ( auto & event : pions )
    {
        for ( auto & pion : event.singlePions )
        {
            pion.pz = ( pion.pz ) * std::sqrt( 1 - b*b ) - b*pion.energy/speedOfLight;
            pion.energy = std::sqrt( 1 - b*b ) * pion.energy - b*speedOfLight*pion.pz;
        }
    }
}