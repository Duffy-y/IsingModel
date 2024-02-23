std::string loadtxt(int **array, uint rows, uint columns) {
    std::string varName = randomString(10);
    std::fstream file;
    file.open("./pyOut/" + varName + ".txt", std::ios::out);
    
    for (uint y = 0; y < rows; y++)
    {
        for (uint x = 0; x < columns; x++)
        {
            file << array[y][x] << " ";
        }
        file << "\n";
    }

    file.close();
    PyRun_SimpleString((varName + " = np.loadtxt('" + varName + ".txt')").c_str());
    return varName;
}

void loadtxt(std::string varName, int **array, uint rows, uint columns) {
    std::fstream file;
    file.open("./pyOut/" + varName + ".txt", std::ios::out);
    
    for (uint y = 0; y < rows; y++)
    {
        for (uint x = 0; x < columns; x++)
        {
            file << array[y][x] << " ";
        }
        file << "\n";
    }

    file.close();
    PyRun_SimpleString((varName + " = np.loadtxt('./pyOut/" + varName + ".txt')").c_str());
}
