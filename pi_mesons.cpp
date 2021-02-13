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

#include "FillContainers.h"

void SetDistrOptions( TH1D* distr )
{
    distr->SetXTitle("Number of pions");
    distr->SetYTitle("Number of cases");
    distr->SetFillColor(54);
    distr->SetMarkerStyle(3);
}

template<typename Number>
void PrintGraph( CArrayWrapper<Number>& xAxis, CArrayWrapper<Number>& yAxis, std::size_t upperBound )
{
    auto c47 = new TCanvas("c47","c47",200,10,600,400);
    c47->Divide(1,2);
    auto gr = new TGraph( upperBound, xAxis.ToArray() , yAxis.ToArray() );
    gr->SetTitle("Exponential distribution of number pions;Number of pions;Number of cases");
    gr->SetFillColor(38);
    gr->SetName("Exponential distribution of number pions");
    c47->cd(1); gr->Draw("AB1");
}

void pi_mesons()
{
    
    std::unique_ptr<TFile> file = std::make_unique<TFile>( "output.root", "RECREATE" );
    double upperBound = 20;
    TH1D* distr = new TH1D( "Exponential distribution of number pions", 
                            "Exponential distribution of number pions", 
                            100, -0.5, upperBound+0.5 );

    int pointNumber;
    std::cout << "Enter number of points of distribution" << std::endl;
    std::cin >> pointNumber;
    
    if ( pointNumber <= 0 )
    {
        throw std::runtime_error( "Number of points must be positive" );
    }

    std::map<int, int> distributionMap_;
    FillDistrMap( distributionMap_, pointNumber, upperBound );
    FillDistr( distr, pointNumber );

    auto xAxis = CArrayWrapper<int>( upperBound + 1 );
    auto yAxis = CArrayWrapper<int>( upperBound + 1 );

    FillXAxis( xAxis, upperBound );
    FillYAxis( yAxis, distributionMap_ );

    SetDistrOptions( distr );

    distr->Write();
    file->cd();
    file->Close();
    
    PrintGraph( xAxis, yAxis, upperBound );

}
