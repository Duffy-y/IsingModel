#include "utils.hpp"

std::string randomString(uint length)
{
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string rand_str;

    for (std::size_t i = 0; i < length; ++i)
    {
        rand_str += CHARACTERS[distribution(generator)];
    }

    return rand_str;
}