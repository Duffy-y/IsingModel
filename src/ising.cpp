#include "ising.hpp"

namespace Ising {

Lattice lattice(const uint sizeX, const uint sizeY) {
    Lattice lat = Lattice();
    lat.spin = (int**)malloc(sizeof(int*) * sizeY);
        
    for (int i = 0; i < sizeY; i++) {
        lat.spin[i] = (int*)malloc(sizeof(int) * sizeX);
    }

    lat.sizeX = sizeX;
    lat.sizeY = sizeY;

    return lat;
}

Lattice lattice(const uint sizeX) {
    return lattice(sizeX, 1);
}

Lattice* latticeMulti(const uint sizeX, const uint sizeY, const uint latticeCount) {
    Lattice *lattices = (Lattice*)malloc(sizeof(Lattice) * latticeCount);
    for (size_t i = 0; i < latticeCount; i++)
    {
        lattices[i] = lattice(sizeX, sizeY);
    }
    return lattices;
}

Lattice* latticeMulti(const uint sizeX, const uint latticeCount) {
    return latticeMulti(sizeX, 1, latticeCount);
}

int* _getSpinRef(Lattice &lat, const int x, const int y) {
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
    for (int y = 0; y < lat.sizeY; y++) {
        for (int x = 0; x < lat.sizeX; x++) {
            setSpin(lat, x, y, spinValue);
        }
    }
}

void randomSpin(Lattice &lat, const double p) {
    assert(p >= 0);
    int spinValue = 0;
    
    for (int y = 0; y < lat.sizeY; y++) {
        for (int x = 0; x < lat.sizeX; x++) {
            spinValue = (double)std::rand() / RAND_MAX < p ? UP : DOWN;
            setSpin(lat, x, y, spinValue);
        }
    }
}

int latticeEnergy(Lattice &lat, int J) {
    int latEnergy = 0;
    for (int y = 0; y < lat.sizeY; y++) {
        for (int x = 0; x < lat.sizeX; x++) {
            latEnergy -= J * getSpin(lat, x, y) * (getSpin(lat, x + 1, y) + getSpin(lat, x, y + 1));
        }
    }
    return latEnergy;
}

int swappingEnergy(Lattice &lat, const int x, const int y, int J) {
    const int neighbor_count = getSpin(lat, x + 1, y) + getSpin(lat, x, y + 1) + getSpin(lat, x - 1, y) + getSpin(lat, x, y - 1);
    return 2 * J * getSpin(lat, x, y) * neighbor_count;
}

double magnetization(Lattice &lat) {
    double mag = 0;
    for (int y = 0; y < lat.sizeY; y++) {
        for (int x = 0; x < lat.sizeY; x++) {
            mag += getSpin(lat, x, y);
        }
    }
    return mag / (lat.sizeY * lat.sizeX);
}

double meanMagnetization(Lattice *lat, uint latticeCount) {
    double mag = 0;
    for (size_t i = 0; i < latticeCount; i++)
    {
        mag += magnetization(lat[i]);
    }
    return mag / latticeCount;
}

int pcx(Lattice &lat, int x) {
    return (x + lat.sizeX) % lat.sizeX;
}

int pcy(Lattice &lat, int y) {
    // * (y + lat.sizeY): petite astuce pour ne pas avoir à définir une méthode de modulo positive.
    return (y + lat.sizeY) % lat.sizeY;
}

}