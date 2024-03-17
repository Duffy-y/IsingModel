#include "ising.hpp"

namespace Ising {

int zero = 0;

Lattice lattice(const uint sizeX, const uint sizeY) {
    Lattice lat = Lattice();
    lat.spin = (int**)malloc(sizeof(int*) * sizeY);
        
    for (uint i = 0; i < sizeY; i++) {
        lat.spin[i] = (int*)malloc(sizeof(int) * sizeX);
    }

    lat.sizeX = sizeX;
    lat.sizeY = sizeY;
    lat.sizeXY = sizeX * sizeY;

    return lat;
}

Lattice lattice(const uint sizeX) {
    return lattice(sizeX, 1);
}

int* _getSpinRef(Lattice &lat, const int x, const int y) {
    // * Dans le cas 1D, cela évite le comptage de la case  (x,y) elle-même à cause de pcy().
    if (lat.sizeY == 1 && y != 0) {
        return &zero;
    }
    return &lat.spin[pcy(lat, y)][pcx(lat, x)];
}

int getSpin(Lattice &lat, const int x, const int y) {
    return *_getSpinRef(lat, x, y);
}

void setSpin(Lattice &lat, const int x, const int y, const int spinValue) {
    *_getSpinRef(lat, x, y) = spinValue;
}

void flipSpin(Lattice &lat, const int x, const int y) {
    *_getSpinRef(lat, x, y) *= -1;
}

void uniformSpin(Lattice &lat, const int spinValue) {
    for (uint y = 0; y < lat.sizeY; y++) {
        for (uint x = 0; x < lat.sizeX; x++) {
            setSpin(lat, x, y, spinValue);
        }
    }
}

void randomSpin(Lattice &lat, const double p) {
    assert(p >= 0);
    int spinValue = 0;
    
    for (uint y = 0; y < lat.sizeY; y++) {
        for (uint x = 0; x < lat.sizeX; x++) {
            spinValue = (double)std::rand() / RAND_MAX < p ? UP : DOWN;
            setSpin(lat, x, y, spinValue);
        }
    }
}

double latticeEnergy(Lattice &lat, double J, double h) {
    int latEnergy = 0;
    for (uint y = 0; y < lat.sizeY; y++) {
        for (uint x = 0; x < lat.sizeX; x++) {
            latEnergy -= J * getSpin(lat, x, y) * ( getSpin(lat, x + 1, y) + getSpin(lat, x, y + 1) );
            latEnergy -= h * getSpin(lat, x, y);
        }
    }
    return latEnergy;
}

double swappingEnergy(Lattice &lat, const int x, const int y, double J, double h) {
    const int neighbor_count = getSpin(lat, x + 1, y) + getSpin(lat, x, y + 1) + getSpin(lat, x - 1, y) + getSpin(lat, x, y - 1);
    return 2 * J * getSpin(lat, x, y) * neighbor_count + 2 * h * getSpin(lat, x, y);
}

double magnetization(Lattice &lat) {
    double mag = 0;
    for (uint y = 0; y < lat.sizeY; y++) {
        for (uint x = 0; x < lat.sizeY; x++) {
            mag += getSpin(lat, x, y);
        }
    }
    return mag / lat.sizeXY;
}

double swappingMagnetization(Lattice &lat, const int x, const int y) {
    return - 2 * getSpin(lat, x, y) / lat.sizeXY;
}

int pcx(Lattice &lat, int x) {
    return (x + lat.sizeX) % lat.sizeX;
}

int pcy(Lattice &lat, int y) {
    // * (y + lat.sizeY): petite astuce pour ne pas avoir à définir une méthode de modulo positive.
    return (y + lat.sizeY) % lat.sizeY;
}
}