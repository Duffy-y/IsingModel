#pragma once

#include <string>
#include <random>

/// @brief Génère une chaine de caractères aléatoires (issu de https://inversepalindrome.com/blog/how-to-create-a-random-string-in-cpp)
/// @param length Longueur de la chaîne de caractères
/// @return Retourne la chaîne de caractères générées.
std::string randomString(uint length);