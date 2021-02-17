#include <iostream>
#include <utility>
#include <cmath>
#include <map>
#include <stdexcept>
#include <vector>

#include <TRandom.h>
#include <TRandom3.h>
#include <TH1D.h>
#include <TFile.h>
#include <TPaletteAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTree.h>

#include "PionsGroup.h"

void FillTree( TTree& tree, const std::vector<PionsGroup>& pions )
{
    Float_t px, py, pz, ip;
    Int_t eventNum = 1;
    tree.Branch("px",&px,"px/F");
    tree.Branch("py",&py,"py/F");
    tree.Branch("pz",&pz,"pz/F");
    tree.Branch("ip",&ip,"ip/F");
    tree.Branch("ev",&eventNum,"ev/I");
    
    for ( const auto& pionGroup: pions )
    {
        for ( const auto& pion: pionGroup.singlePions )
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
    std::cout << "Enter the number of distribution points" << std::endl;
    std::cin >> pointNumber;

    if ( pointNumber <= 0 )
    {
        throw std::runtime_error( "Number of points must be positive" );
    }
    if ( pointNumber > 100000 )
    {
        throw std::runtime_error( "Too many points" );
    }

    auto pions = std::vector<PionsGroup>( pointNumber );

    for ( int i = 0; i < pointNumber; ++i )
    {
        int numberOfPions = round( rnd->Exp( 4 ) );

        // Заполняем значения события, количество частиц в котором равно 
        // полученному случайным образом по экспоненциальному распределению числу
        // Импульс и энергия каждой частицы в каждой из групп также выбирается случайно:
        // Энергия - по экспоненциальному закону с параметром 1000 (в МЭв),
        // Импульс - по равномерно распределенной сфере с радиусом sqrt( 2 * 139.5 * величина энергии )
        pions[i] = PionsGroup( numberOfPions );
    }

    // Записываем полученные значения 4-х импульса в дерево
    TTree tree("tree", "4-x momentum");
    FillTree(tree, pions);

    tree.Write();
}