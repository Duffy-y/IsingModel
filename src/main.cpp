#include <Python.h>
#include "pythoncpp.hpp"
#include "colormap.hpp"
#include "ising.hpp"
#include "montecarlo.hpp"
#include <ctime>
#include <fstream>
#include <iostream>

void showAlgorithm(Ising::Lattice &lat, MC::Parameters options) {
    plt::ion();

    auto spin = np::array(lat.spin, lat.sizeY, lat.sizeX);
    double deltaE = 0;
    double deltaM = 0;
    for (size_t i = 0; i < options.epochThreshold; i++)
    {
        if (i % options.jumpSize == 0) {
            plt::clf();
            plt::xlabel("X");
            plt::ylabel("Y");
            plt::title(std::to_string(i) + ", Xi = " + std::to_string(Ising::magnetization(lat)));
            np::array(spin, lat.spin, lat.sizeY, lat.sizeX);
            plt::imshow(spin, CMAP_seismic);
            plt::colorbar(); 
            plt::pause();
        }
        options.mcIterator(lat, options, deltaE, deltaM);
    }

    plt::ioff();
}

int main(int argc, char *argv[]) {
    std::srand(std::time(NULL));
    py::openPython();    

    // * Initialisation du réseau de spin
    Ising::Lattice lat = Ising::lattice(15, 15);
    Ising::randomSpin(lat, 0.5);

    // * Création des paramètres de simulation
    MC::Parameters options = MC::parameters(5e6, 150000, 0.5, 0.0002, MC::metropolisIteration, 0.1, 1, 0, 1);
    // MC::Parameters options = MC::parameters(500, 50, 0.5, 0.0002, MC::wolffIteration, 0.1, 1, 0, 1);

    uint samplingPoints = 100;
    MC::Properties props = MC::thermalizeLattice(lat, options, 0.1, 3, samplingPoints);
    
    auto E = np::array(props.E, samplingPoints);
    auto E_sq = np::array(props.E_sq, samplingPoints);
    auto M = np::array(props.M, samplingPoints);
    auto M_sq = np::array(props.M_sq, samplingPoints);
    auto M_abs = np::array(props.M_abs, samplingPoints);
    auto T = np::array(props.T, samplingPoints);
    
    np::savetxt(E, "E.csv", ";");
    np::savetxt(E_sq, "E_sq.csv", ";");
    np::savetxt(M, "M.csv", ";");
    np::savetxt(M_sq, "M_sq.csv", ";");
    np::savetxt(M_abs, "M_abs.csv", ";");
    np::savetxt(T, "T.csv", ";");

    py::closePython();
    return 0;
}   