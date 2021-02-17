struct SinglePion
{
    double energy;
    double px;
    double py;
    double pz;

    SinglePion() = default;
    SinglePion( const SinglePion& other )
    :energy( other.energy ), px( other.px ), py( other.py ), pz( other.pz )
    {}

    SinglePion& operator=( const SinglePion& other )
    {
        if ( this == &other )
        {
            return *this;
        }
        energy = other.energy;
        px = other.px;
        py = other.py;
        pz = other.pz;

        return *this;
    }

    SinglePion( double energy_, double px_, double py_, double pz_ )
    :energy( energy_), px( px_ ), py( py_), pz( pz_ )
    {}

};

