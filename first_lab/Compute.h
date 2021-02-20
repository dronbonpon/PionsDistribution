#include <vector>

#include <TRandom.h>
#include <TRandom3.h>

#include "thread_RAII.h"
#include "PionsEvent.h"

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
    int numberOfTasks = 8;
    
    std::vector<ThreadRAII> threads;

    int sizeOfSequentialTask = ( pions.size() % 6 == 0 ) ? 
                                pions.size()/numberOfTasks : pions.size()/numberOfTasks + 1;

    int i = sizeOfSequentialTask;
    
    while ( i < pions.size() )
    {
        ThreadRAII currentThread( std::thread( ComputeSequentially, std::ref( pions ), i-sizeOfSequentialTask, i ), ThreadRAII::DtorAction::join );
        threads.emplace_back( std::move( currentThread ) );
        i += sizeOfSequentialTask;
    }

    i -= sizeOfSequentialTask;
    ThreadRAII currentThread( std::thread( ComputeSequentially, std::ref( pions ), i, pions.size() ), ThreadRAII::DtorAction::join );
    threads.emplace_back( std::move( currentThread ) );

}


void Compute( std::vector<PionsEvent>& pions )
{
    if ( pions.size() > 100000 )
    {
        return ComputeParallel( pions );
    }
    return ComputeSequentially( pions, 0, pions.size() );
}