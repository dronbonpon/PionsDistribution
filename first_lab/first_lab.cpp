#include <iostream>
#include <utility>
#include <cmath>
#include <map>
#include <stdexcept>
#include <vector>
#include <chrono>

#include <TRandom.h>
#include <TRandom3.h>
#include <TH1D.h>
#include <TFile.h>
#include <TPaletteAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTree.h>

#include "Compute.h"

void SpeedTest( std::vector<PionsEvent>& pions )
{
    using namespace std::chrono;
    int pointNumber = 1000000;

    high_resolution_clock::time_point parallelStart = high_resolution_clock::now();
    ComputeParallel( pions );
    high_resolution_clock::time_point parallelEnd = high_resolution_clock::now();
    duration<double, std::milli> parallelTimeSpan = parallelEnd - parallelStart;
    std::cout << "Computing " << pointNumber << " points" << std::endl;
    std::cout << "Parallel time: " << parallelTimeSpan.count() << std::endl;

    high_resolution_clock::time_point seqStart = high_resolution_clock::now();
    ComputeSequentially( pions, 0, pions.size() );
    high_resolution_clock::time_point seqEnd = high_resolution_clock::now();
    duration<double, std::milli> seqTimeSpan = seqEnd - seqStart;
    std::cout << "Sequential time: " << seqTimeSpan.count() << std::endl;
}

void FillTree( TTree& tree, const std::vector<PionsEvent>& pions )
{
    Float_t px, py, pz, ip;
    Int_t eventNum = 1;
    tree.Branch("px",&px,"px/F");
    tree.Branch("py",&py,"py/F");
    tree.Branch("pz",&pz,"pz/F");
    tree.Branch("ip",&ip,"ip/F");
    tree.Branch("ev",&eventNum,"ev/I");
    
    for ( const auto& pionEvent: pions )
    {
        for ( const auto& pion: pionEvent.singlePions )
        {
            px = pion.px;
            py = pion.py;
            pz = pion.pz;
            ip = pion.energy;
            tree.Fill();
        }
        ++eventNum;
    }
}

void first_lab()
{
    std::unique_ptr<TRandom> rnd = std::make_unique<TRandom3>();
    TFile file("tree.root", "recreate");
    int pointNumber;
    std::cout << "Enter the number of events: ";
    std::cin >> pointNumber;

    bool parallelComputing = false;

    if ( pointNumber <= 0 )
    {
        throw std::runtime_error( "Number of points must be positive" );
    }
    if ( pointNumber >= 100000 && pointNumber <= 4000000 )
    {
        parallelComputing = true;
    }
    if ( pointNumber > 2000000 )
    {
        throw std::runtime_error( "Too many points" );
    }

    auto pions = std::vector<PionsEvent>( pointNumber );

    if ( parallelComputing )
    {
        ComputeParallel( pions );
    }
    else
    {
        ComputeSequentially( pions, 0, pions.size() );
    }

    // Раскомментировать, если хотите провести сравнение скорости 
    // параллельных и последовательных вычислений
    // В таком слуае, желательно закомментировать строки 90, 94 и 106
    // ( Чтобы программа исполнялась быстрее )

    //SpeedTest( pions );

    // Записываем полученные значения 4-х импульса в дерево
    TTree tree("tree", "4-x momentum");
    FillTree(tree, pions);

    tree.Write();

    std::cout << pointNumber << " events were successfully generated and written to the tree.root" << std::endl;
}