#include <Python.h>
#include "pythoncpp.hpp"
#include "colormap.hpp"
#include "ising.hpp"
#include "montecarlo.hpp"
#include <ctime>
#include <fstream>
#include <iostream>

// TODO: Pythoncpp -> fonction pour faire des sous-figures facilement
// TODO: thermalizeLattice -> affichage en temps réel du réseau
// TODO: Une fonction qui plot les grandeurs moyennées.
// TODO: PDF d'explication du Projet
// TODO: Powerpoint de présentation

#define REAL_TIME 0

void showAlgorithm(Ising::Lattice &lat, MC::Options options) {
    auto spin = np::loadtxt(lat.spin, lat.sizeY, lat.sizeX);
    for (size_t i = 0; i < options.epochThreshold; i++)
    {
        if (i % options.jumpSize == 0) {
            plt::clf();
            plt::xlabel("X");
            plt::ylabel("Y");
            plt::title(std::to_string(i));
            np::loadtxt(spin, lat.spin, lat.sizeY, lat.sizeX);
            plt::imshow(spin, cmap_seismic);
            plt::pause();
        }
        options.mcIterator(lat, options);
    }
}

int main(int argc, char *argv[]) {
    plt::openPython(REAL_TIME);
    std::srand(std::time(NULL));
    
    Ising::Lattice lat = Ising::lattice(100, 100);
    Ising::uniformSpin(lat, 1);

    MC::Options options = MC::Options();
    options.J = 1;
    options.jumpSize = 1000;
    options.T = 0.1;
    options.kB = 1;
    options.epochThreshold = 1000000;
    options.relativeVariation = 0.002; // Valeur raisonnable (~ 300 iterations Wolff pour un équilibre correct)
    options.mcIterator = MC::metropolisIteration;
    options.dataRecordDuration = 1;

    // showAlgorithm(lat, options);

    // MC::Properties result = MC::thermalizeLattice(lat, options, 0.1, 3, 30);

    // auto T = np::loadtxt(result.T, 40);
    // auto m = np::loadtxt(result.M_abs, 40);


    // plt::plot(T, m, "Magnetization");
    // plt::show();
    
    plt::closePython();
    return 0;
}