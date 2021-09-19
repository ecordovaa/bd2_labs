#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "lib.hpp" 
using namespace std;

class IsamFile {
private:
    const string fileName = "Data/isam.dat";
    // -- Declarar los indices (crecen hasta 3 veces) --
    // -- Declarar factor de bloque --
public:
    IsamFile();
    void add(Record record);
    Record search(const int& key);
    vector<Record> rangeSearch(int begin, int end);
};

// O(n/BLOCK_SIZE): Reduce los accesos a memoria llenando blockes, no unidades.
void IsamFile::add(Record record){

}

// O(lgn/BLOCK_SIZE): Busqueda binaria + técnica de páginacion por bloques.
Record IsamFile::search(const int& key){

}

// O(lgn/BLOCK_SIZE + n): Búsqueda individual + más una cantidad arbitraria de registros
// Note que el número de overflow pages podría ser incluso mayor que n.
// En ese caso extremo, este algoritmo es tan poco eficiente como una búsqueda lineal.
vector<Record> IsamFile::rangeSearch(int begin, int end){

}