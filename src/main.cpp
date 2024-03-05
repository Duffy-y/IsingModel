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

void showAlgorithm(Ising::Lattice &lat, MC::Parameters options) {
    auto spin = np::array(lat.spin, lat.sizeY, lat.sizeX);
    for (size_t i = 0; i < options.epochThreshold; i++)
    {
        if (i % options.jumpSize == 0) {
            plt::clf();
            plt::xlabel("X");
            plt::ylabel("Y");
            plt::title(std::to_string(i) + ", Xi = " + std::to_string(Ising::magnetization(lat)));
            np::array(spin, lat.spin, lat.sizeY, lat.sizeX);
            plt::imshow(spin, cmap_seismic);
            plt::pause();
        }
        options.mcIterator(lat, options);
    }
}

int main(int argc, char *argv[]) {
    PyObject *m = py::openPython();
    plt::ion();
    std::srand(std::time(NULL));
    
    Ising::Lattice lat = Ising::lattice(25, 25);
    Ising::randomSpin(lat, 0.5);

    MC::Parameters options = MC::Parameters();
    options.J = 1;
    options.jumpSize = 50000;
    options.T = 0.1;     
    options.kB = 1;
    options.epochThreshold = 2000000;
    options.relativeVariation = 0.0002; // Valeur raisonnable (~ 300 iterations Wolff pour un équilibre correct)
    options.mcIterator = MC::metropolisIteration;
    options.dataRecordDuration = 1;

    // showAlgorithm(lat, options);

    py::closePython();
    return 0;
}