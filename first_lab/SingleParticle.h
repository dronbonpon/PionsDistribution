#pragma once

/// Структура отдельной частицы с полями
/// энергия, импульсы, быстрота, псевдобыстрота и азимутальный угол
struct SingleParticle
{
    double energy;
    double px;
    double py;
    double pz;
    double rapidity = 0;
    double pseudorapidity = 0;
    double azimuthalAngle = 0;

    SingleParticle() = default;
    SingleParticle( const SingleParticle& other ) = default;
    SingleParticle& operator=( const SingleParticle& other ) = default;
    
    SingleParticle( double energy_, double px_, double py_, double pz_ )
    :energy( energy_), px( px_ ), py( py_), pz( pz_ )
    {}

    void SetRapidity( double rapidity_ )
    {
        rapidity = rapidity_;
    }

    void SetPseudorapidity( double pseudorapidity_ )
    {
        pseudorapidity = pseudorapidity_;
    }

    void SetAzimuthalAngle( double azimuthalAngle_ )
    {
        azimuthalAngle = azimuthalAngle_;
    }

};

