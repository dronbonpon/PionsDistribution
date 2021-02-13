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

#include "../FillDistrMap.h"

template<typename Pred>
void FillDistr( TH1D* distr, std::size_t pointNumber, Pred pred, int param )
{
    for ( int i = 0; i < pointNumber; ++i )
    {
        int x = round( pred( param ) );

        distr->Fill(x);
    }
}

void SetDistrOptions( TH1D* distr )
{
    distr->SetXTitle("Energy, 10^8");
    distr->SetYTitle("Number of cases");
    distr->SetFillColor(54);
    distr->SetMarkerStyle(10);
}

template<typename Number>
void PrintGraph( CArrayWrapper<Number>& xAxis, CArrayWrapper<Number>& yAxis, std::size_t upperBound )
{
    auto c47 = new TCanvas("c47","c47",200,10,600,400);
    c47->Divide(1,2);
    auto gr = new TGraph( upperBound, xAxis.ToArray() , yAxis.ToArray() );
    gr->SetTitle("Exponential distribution of pions energy;Energy of pions, 10^8;Number of cases");
    gr->SetFillColor(38);
    gr->SetName("Exponential distribution of pions energy");
    c47->cd(1); gr->Draw("AC");
}

void pi_mesons_energy()
{
    
    std::unique_ptr<TFile> file = std::make_unique<TFile>( "output.root", "RECREATE" );
    std::unique_ptr<TRandom> rnd = std::make_unique<TRandom3>();
    long double upperBound = 35;
    TH1D* distr = new TH1D( "Exponential distribution of pions energy", 
                            "Exponential distribution of pions energy", 
                            100, -0.5, upperBound + 0.5 );

    int pointNumber;
    std::cout << "Enter the number of distribution points" << std::endl;
    std::cin >> pointNumber;
    
    if ( pointNumber <= 0 )
    {
        throw std::runtime_error( "Number of points must be positive" );
    }
    if ( pointNumber > 5000000)
    {
        throw std::runtime_error( "Too many points" );
    }

    std::map<double, int> distributionMap_;
    FillDistrMap( distributionMap_, pointNumber, upperBound, 
    [&rnd]( int param )
    {
        return rnd->Exp( param );
    }, 10 );
    FillDistr( distr, pointNumber,
    [&rnd]( int param )
    {
        return rnd->Exp( param );
    }, 10 );

    auto xAxis = CArrayWrapper<double>( upperBound + 1 );
    auto yAxis = CArrayWrapper<double>( upperBound + 1 );

    FillXAxis( xAxis, upperBound );
    FillYAxis( yAxis, distributionMap_ );

    SetDistrOptions( distr );

    distr->Write();
    file->cd();
    file->Close();
    
    PrintGraph( xAxis, yAxis, upperBound );

}