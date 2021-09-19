#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "lib.hpp" 
#define HEAP_LIMIT 5 // Tamaño de aux
using namespace std;

//NOTA: Para simplificar el código, el 'archivo' aux se comporta como un heap en el mismo main file
//La idea es unir ambas partes del main file (ordered y heap) cada cierto tiempo

class SequentialFile {
private:
    const string fileName = "Data/seqfile.dat";
    int size;
    int heapSize;
    int binarySearch(const int& key);
    void reorganize();
public:
    SequentialFile();
    void insertAll(vector<SeqFile::Record> records);
    void add(SeqFile::Record record);
    SeqFile::Record search(const int& key);
    vector<SeqFile::Record> rangeSearch(int begin, int end);
    ~SequentialFile() {if (heapSize > 0) reorganize();};
};

SequentialFile::SequentialFile(){
    ifstream file(fileName, ios::binary);
    if(!file.is_open()){
        cout << "No se puede abrir/encontrar el archivo, abortando el programa.\n";
        exit(-1);
    }
    file.seekg(0, ios::end);
    size = file.tellg();
    size /= sizeof(SeqFile::Record);
}

// O(n): Se insertán n registros en el archivo.
void SequentialFile::insertAll(vector<SeqFile::Record> records){
    sort(records.begin(), records.end(), SeqFile::recordComp); 

    // Advierta que si el usuario usa esta función se sobrescribirán sus archivos previos
    ofstream file(fileName, ios::binary | ios::trunc);
    size = 0;
    file.seekp(0, ios::beg);
    if (!file.is_open()) {
        cout << "No se puede abrir/encontrar el archivo, la operación insertAll no pudo realizarse.\n";
        return;
    }
    for (auto record: records){
        record.next = file.tellp();
        record.next += sizeof(SeqFile::Record); //Posición física
        file.write((char*) &record, sizeof(SeqFile::Record));
        size++;
    }
    file.seekp((size+1) * sizeof(SeqFile::Record) - sizeof(int));
    int endOfFile = -1;
    file.write((char*) &endOfFile, sizeof(int));
    file.close();
}

//O(lgn + k): Busqueda binaria + busqueda secuencial en k o menos elementos.
void SequentialFile::add(SeqFile::Record record){
    fstream file(fileName, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        cout << "No se puede abrir/encontrar el archivo, la operación insertAll no pudo realizarse.\n";
        return;
    }
    int searched = binarySearch(record.codigo);
    int searchedKey {};
    file.seekg(searched * sizeof(SeqFile::Record));

    // Nos aseguramos que el registro no este insertado
    file.read((char*) &searchedKey, sizeof(int));
    if(searchedKey == record.codigo){
        file.close();
        cout << "Ya existe un registro con código " << searchedKey <<  ", inserción rechazada.\n";
        return;
    }

    // Busqueda secuencial en aux
    int nextIdx = searched;
    do {
        file.seekg((nextIdx + 1) * sizeof(SeqFile::Record) - sizeof(int));
        searched = nextIdx;
        file.read((char*) &nextIdx, sizeof(int));
    } while(nextIdx < record.codigo);

    // Actualización de next y size
    record.next = nextIdx;
    file.seekg((searchedKey + 1) * sizeof(SeqFile::Record) - sizeof(int));
    file.write((char*) &record.codigo, sizeof(int));
    file.seekg(0, ios::end);
    file.write((char*) &record, sizeof(SeqFile::Record));
    heapSize++;
    file.close();

    if(heapSize == HEAP_LIMIT) reorganize();
}

//O(lgn + k): Se usa búsqueda binaria, pero se busca secuencialmente luego de una búsqueda fallida
//Si k < C*lgn (con C constante), entonces podemos decir incluso que es O(lgn)
SeqFile::Record SequentialFile::search(const int& key){
    SeqFile::Record record;

    // Busca en main
    int recordKey = binarySearch(key);   
    if (recordKey != key){

        // Busca en aux (registros insertados despues del último insertAll)
        if(heapSize == 0){
            cout << "ERROR: No se encontro el registro.\n";
            return record;
        }
        ifstream file(fileName, ios::binary);
        if (!file.is_open()) {
            cout << "No se puede abrir/encontrar el archivo, la operación insertAll no pudo realizarse.\n";
            return record;
        }
        file.seekg((size + heapSize + 1) * sizeof(SeqFile::Record));
        while(file.read((char*) &record, sizeof(SeqFile::Record))){
            if(record.codigo == key){
                file.close();  
                return record;
            }
        }
        file.close();
        cout << "ERROR: No se encontro el registro.\n";
        return record;
    }                            
    ifstream file (fileName, ios::binary);
    if (!file.is_open()) {
        cout << "No se puede abrir/encontrar el archivo, la operación insertAll no pudo realizarse.\n";
        return record;
    }
    file.seekg(recordKey * sizeof(SeqFile::Record));
    file.read((char*) &record, sizeof(SeqFile::Record));
    file.close();
    return record;
}

//O(lgn + n): Costo de search más procesar secuencialmente n o menos registros
vector<SeqFile::Record> SequentialFile::rangeSearch(int begin, int end){
    SeqFile::Record record = search(min(begin,end));
    if (record.codigo != begin) 
        cout<< "ERROR: El indice referenciado por begin no se pudo localizar.\n";
    vector<SeqFile::Record> inRange = {record};
    end = max(begin,end);
    ifstream file(fileName, ios::binary);
    if (!file.is_open()) {
        cout << "No se puede abrir/encontrar el archivo, la operación insertAll no pudo realizarse.\n";
        return {};
    }
    while (record.next <= end){
        file.seekg(record.next * sizeof(SeqFile::Record));
        file.read((char*) &record, sizeof(SeqFile::Record));
        inRange.push_back(record);
    }
    file.close();
    return inRange;
}

// -- Otras funciones --

// Retorna el registro si lo encuentra 
int SequentialFile::binarySearch(const int& key){
    ifstream file(fileName, ios::binary);
    int l {}, u = size - 1, pos {};
    while (l <= u){
        int m = (u + l) / 2;
        file.seekg(m * sizeof(SeqFile::Record));
        file.read((char*) &pos, sizeof(int));
        if (pos == key) {
            file.close();
            return m;
        } 
        else if (key <pos) u = m - 1;
        else l = m + 1;
    }
    file.close();
    return max(u,0);
}

// TODO: Resetea el archivo cuando llega a un límite o cuando acaba el programa
void SequentialFile::reorganize(){
    // Cargamos todos los datos del archivo (main + aux)
    vector<SeqFile::Record> all {};
    SeqFile::Record record;
    fstream file(fileName, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        cout << "El archivo debe reorganizarse, pero no ha podido ser abierto para ello. Se cerrará la sesión.\n";
        exit(-1);
    }
    while(file.read((char*) &record, sizeof(SeqFile::Record))) all.push_back(record);

    // Reinsertamos los registros ya ordenados
    insertAll(all);
}