struct SinglePion
{
    double energy;
    double px;
    double py;
    double pz;

    SinglePion() = default;
    SinglePion( const SinglePion& other ) = default;
    SinglePion& operator=( const SinglePion& other ) = default;
    
    SinglePion( double energy_, double px_, double py_, double pz_ )
    :energy( energy_), px( px_ ), py( py_), pz( pz_ )
    {}

};

