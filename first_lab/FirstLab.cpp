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
#include <TStyle.h>

#include "Compute.h"
#include "PionsEvent.h"

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
        for ( const auto& pion: pionEvent.singleParticles )
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
    TTree* tree = (TTree*)file->Get( treeName.c_str() );

    tree->SetBranchAddress("px", &px);
    tree->SetBranchAddress("py", &py);
    tree->SetBranchAddress("pz", &pz);
    tree->SetBranchAddress("ip", &ip);
    tree->SetBranchAddress("ev", &eventNum);
    
    tree->GetEntry( 0 );

    Int_t nentries = ( Int_t )( tree->GetEntries() );
    int currentEvent, numberOfPions = 0;
    int previousEvent = eventNum;

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
            numberOfPions++;
        }
        else
        {
            PionsEvent event( MomentumVector( pxVector, pyVector, pzVector, ipVector ), numberOfPions );
            pions.push_back( event );
            pxVector.clear();
            pyVector.clear();
            pzVector.clear();
            ipVector.clear();
            numberOfPions = 0;
            previousEvent = eventNum;
        }
    }
    PionsEvent event( MomentumVector( pxVector, pyVector, pzVector, ipVector ), numberOfPions );
    pions.push_back( event );
}

void SetDistrOptions( TH1D* distr, const std::string& xAxis, const std::string& yAxis,
                      int fillColor = 54, int markerStyle = 20 )
{
    distr->SetXTitle( xAxis.c_str() );
    distr->SetYTitle( yAxis.c_str() );
    distr->SetFillColor( fillColor );
    distr->SetMarkerStyle( markerStyle );
    distr->SetStats( false );
    gStyle->SetEndErrorSize(3);
    gStyle->SetErrorX(1.);
}

std::pair<double, double> MaxRapidityAndPseudoRapidity( const std::vector<PionsEvent>& pions )
{
    double maxRapidity = 0;
    double maxPseudoRapidity = 0;
    for ( const auto & event : pions )
    {
        for ( const auto & pion : event.singleParticles )
        {
            if ( pion.rapidity > maxRapidity )
            {
                maxRapidity = pion.rapidity;
            }
            if ( pion.pseudorapidity > maxPseudoRapidity )
            {
                maxPseudoRapidity = pion.pseudorapidity;
            }
        }
    }
    return std::make_pair( maxRapidity, maxPseudoRapidity );
}

void FillDistr( TH1D* rapidity, TH1D* pseudorapidity, const std::vector<PionsEvent>& pions )
{
    std::pair<double, double> maxValues = MaxRapidityAndPseudoRapidity( pions );
    for ( const auto & event : pions )
    {
        for ( const auto & pion : event.singleParticles )
        {
            rapidity->Fill( maxValues.first - pion.rapidity );
            pseudorapidity->Fill( maxValues.second - pion.pseudorapidity );
        }
    }
}

void FillDistr( TH1D* rapidityDistrBefore, TH1D* rapidityDistrAfter, 
                TH1D* pseudorapidityDistrBefore, TH1D* pseudorapidityDistrAfter,
                const std::vector<PionsEvent>& pions, const std::vector<PionsEvent>& pionsAfterLorentz )
{
    SetDistrOptions( rapidityDistrBefore, "Rapidity", "Number of cases" );
    SetDistrOptions( rapidityDistrAfter, "Rapidity", "Number of cases" );

    SetDistrOptions( pseudorapidityDistrBefore, "Pseudoapidity", "Number of cases" );
    SetDistrOptions( pseudorapidityDistrAfter, "Pseudoapidity", "Number of cases" );

    FillDistr( rapidityDistrBefore, pseudorapidityDistrBefore, pions );
    
    for ( int i  = 0; i < 5; i++ )
    {
        FillDistr( rapidityDistrAfter, pseudorapidityDistrAfter, pionsAfterLorentz );
    }

}

struct HistOptions
{
    int nbinsx = 100;
    double xLow = 0;
    double xUp = 5.5;
};

TH1D* CreateHist( const std::string& histName, HistOptions histOptions = {} )
{
    return new TH1D( histName.c_str(), histName.c_str(),
                     histOptions.nbinsx, histOptions.xLow, histOptions.xUp );
}

void FirstLab()
{
    int eventNumber;
    std::cout << "Enter the number of events: ";
    std::cin >> eventNumber;

    if ( eventNumber <= 0 )
    {
        throw std::runtime_error( "Number of events must be positive" );
    }
    if ( eventNumber > 2000000 )
    {
        throw std::runtime_error( "Too many events" );
    }

    TFile file("tree.root", "recreate");

    auto pions = std::vector<PionsEvent>( eventNumber );

    // Заполняем события рождения пионов
    Compute( pions );

    // Записываем полученные значения 4-х импульса в дерево
    TTree tree( "tree", "4-x momentum" );
    FillTree( tree, pions );

    tree.Write();

    std::cout << eventNumber << " events were successfully generated and written to the tree.root" << std::endl;

    file.Close();

    // Читаем сгенерированные события из дерева в вектор pionsFromTree
    std::vector<PionsEvent> pionsFromTree;
    ReadTree( "tree.root", "tree", pionsFromTree );

    std::cout << eventNumber << " events were successfully readed from the tree.root" << std::endl;

    // Вычисляем быстроту, псевдобыстроту и азимутальный угол каждой частицы
    ComputeParams( pionsFromTree );

    // Преобразования Лоренца
    std::vector<PionsEvent> pionsFromTreeAfterLorentz = pionsFromTree;
    LorentzTransformation( pionsFromTreeAfterLorentz, 0.95 );

    // Вычисляем быстроту и псевдобыстроту частиц после преобразования Лоренца
    ComputeParams( pionsFromTreeAfterLorentz );

    TFile outputFile( "OutputHists.root", "RECREATE" );

    // Рисуем графики
    TH1D* rapidityDistrAfter = CreateHist( "Rapidity distribution of pions before Lorentz transformation" );

    TH1D* rapidityDistrBefore = CreateHist( "Rapidity distribution of pions after Lorentz transformation" );

    TH1D* pseudorapidityDistrAfter = CreateHist( "Pseudorapidity distribution of pions before Lorentz transformation" );

    TH1D* pseudorapidityDistrBefore = CreateHist( "Pseudorapidity distribution of pions after Lorentz transformation" );
    
    FillDistr( rapidityDistrBefore, rapidityDistrAfter, pseudorapidityDistrBefore, 
               pseudorapidityDistrAfter, pionsFromTree, pionsFromTreeAfterLorentz );
    
    rapidityDistrBefore->Write();
    pseudorapidityDistrBefore->Write();

    rapidityDistrAfter->Write();
    pseudorapidityDistrAfter->Write();

    std::cout << "Histogram were successfully created and writed to OutputHists.root" << std::endl;

}