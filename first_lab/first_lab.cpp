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

void ReadTree( const std::string& fileName, const std::string& treeName,
               std::vector<PionsEvent>& pions )
{
    Float_t px, py, pz, ip;
    Int_t eventNum = 1;
    TFile* file = new TFile( fileName.c_str() );
    TTree* tree = (TTree*)file->Get("tree");

    tree->SetBranchAddress("px", &px);
    tree->SetBranchAddress("py", &py);
    tree->SetBranchAddress("pz", &pz);
    tree->SetBranchAddress("ip", &ip);
    tree->SetBranchAddress("ev", &eventNum);
    
    tree->GetEntry( 0 );

    Int_t nentries = ( Int_t )( tree->GetEntries() );
    int currentEvent, numberOfPions = 0;
    int previousEvent = eventNum;
    std::cout << nentries << std::endl;
    std::vector<Float_t> pxVector;
    std::vector<Float_t> pyVector;
    std::vector<Float_t> pzVector;
    std::vector<Float_t> ipVector;
    
    for ( int i = 0; i < nentries; i++ )
    {
        tree->GetEntry( i );
        if ( eventNum == previousEvent )
        {
            pxVector.push_back( px );
            pyVector.push_back( py );
            pzVector.push_back( pz );
            ipVector.push_back( ip );
            //std::cout << px << " " << py << " " << pz << " " << ip << std::endl;
            numberOfPions++;
        }
        else
        {
            PionsEvent event( pxVector, pyVector, pzVector, ipVector, numberOfPions );
            pions.push_back( event );
            pxVector.clear();
            pyVector.clear();
            pzVector.clear();
            ipVector.clear();
            numberOfPions = 0;
            previousEvent = eventNum;
        }
    }
    PionsEvent event( pxVector, pyVector, pzVector, ipVector, numberOfPions );
    pions.push_back( event );
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

    // Заполняем события рождения пионов
    Compute( pions );

    // Записываем полученные значения 4-х импульса в дерево
    TTree tree("tree", "4-x momentum");
    FillTree(tree, pions);

    tree.Write();

    std::cout << pointNumber << " events were successfully generated and written to the tree.root" << std::endl;

    file.Close();

    // Читаем сгенерированные события из дерева в вектор pionsFromTree
    std::vector<PionsEvent> pionsFromTree;
    ReadTree("tree.root", "tree", pionsFromTree );

}