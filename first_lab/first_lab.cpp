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
    TFile file("tree.root", "recreate");
    int pointNumber;
    std::cout << "Enter the number of events: ";
    std::cin >> pointNumber;

    if ( pointNumber <= 0 )
    {
        throw std::runtime_error( "Number of points must be positive" );
    }
    if ( pointNumber > 2000000 )
    {
        throw std::runtime_error( "Too many points" );
    }

    auto pions = std::vector<PionsEvent>( pointNumber );

    Compute( pions );

    // Записываем полученные значения 4-х импульса в дерево
    TTree tree("tree", "4-x momentum");
    FillTree(tree, pions);

    tree.Write();

    std::cout << pointNumber << " events were successfully generated and written to the tree.root" << std::endl;
}