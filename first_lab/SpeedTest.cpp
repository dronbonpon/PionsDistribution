#include <iostream>

#include <TRandom.h>
#include <TRandom3.h>

#include "Compute.h"


void SpeedTest()
{
    using namespace std::chrono;
    int pointNumber = 1000000;
    std::vector<PionsEvent> pions( pointNumber );

    high_resolution_clock::time_point parallelStart = high_resolution_clock::now();
    ComputeParallel( pions );
    high_resolution_clock::time_point parallelEnd = high_resolution_clock::now();
    duration<double, std::milli> parallelTimeSpan = parallelEnd - parallelStart;
    std::cout << "Computing one million distribution points" << std::endl;
    std::cout << "Parallel computing time: " << parallelTimeSpan.count() << " ms" << std::endl;

    high_resolution_clock::time_point seqStart = high_resolution_clock::now();
    ComputeSequentially( pions, 0, pions.size() );
    high_resolution_clock::time_point seqEnd = high_resolution_clock::now();
    duration<double, std::milli> seqTimeSpan = seqEnd - seqStart;
    std::cout << "Sequential computing time: " << seqTimeSpan.count() << " ms" << std::endl;
}