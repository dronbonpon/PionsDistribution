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
            double rapidity = std::log( ( pion.energy + absMomentum )/( pion.energy - absMomentum ) )/2;
            pion.SetRapidity( rapidity );
            double pseudorapidity = std::log( ( pion.energy + pion.pz )/( pion.energy - pion.pz ) )/2;
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
    long int speedOfLight = 1;
    for ( auto & event : pions )
    {
        for ( auto & pion : event.singlePions )
        {
            pion.pz = ( pion.pz + b*pion.energy/speedOfLight )/std::sqrt( 1 - b*b );
            pion.energy = ( pion.energy + b*speedOfLight*pion.pz )/std::sqrt( 1 - b*b );
        }
    }
}