#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "lib.hpp" 
using namespace std;
#define TAM_PAGE 4096


//" Constantes M y K {{{1
constexpr int M = (TAM_PAGE - sizeof(long))/(sizeof(long) + sizeof(int));
constexpr int K = (TAM_PAGE - sizeof(long) - sizeof(int))/sizeof(Record);


//" Binary search maleado {{{1
int closeBinarySearch(int arr[], int l, int r ,int x){
    if(r > l){
        int mid = l+(r-l)/2;
        if(arr[mid] == x)
            return mid;
        if(arr[mid] > x)
            return closeBinarySearch(arr, l, mid -1 , x);
        return closeBinarySearch(arr, mid + l, r, x);
    }
        return l;
}


//" Bucket {{{1
struct Bucket{
    int keys[M];
    long pages[M+1];
    
    //Bucket() {{{2
    Bucket(){}
    //" Bucket(fileName, position) {{{2
    Bucket(string fileName, long position=0){
        fstream file;
        file.open(fileName, fstream::in | fstream::binary);
        file.seekg(position, ios::beg);
        for(int i = 0; i < 2*M +1;i++){
            if(!i%2)
                file.read((char*)&pages[i/2], sizeof(pages[i/2]));
            else 
                file.read((char*)&keys[i/2], sizeof(pages[i/2]));
        }
        file.close();
    } 
    //" findNextBucket(key) {{{2
    long findNextBucket(int key){
        return pages[closeBinarySearch(keys,0,M,key)]; 
    }
};

//" DataPage {{{1
struct DataPage{
    Record records[K];
    int size;
    long nextPage = -1;
    //" DataPage() {{{2
    DataPage(){}
    //" DataPage(fileName, position) {{{2
    DataPage(string fileName, long position=0){
        fstream file;
        file.open(fileName, fstream::in | fstream::binary);
        file.seekg(position, ios::beg);
        for(int i = 0; i<K; i++)
            file.read((char*)&records[i], sizeof(records[i]));
        file.close();
    }
    //" bringRecord(key) {{{2
    Record bringRecord(int key){
        for(int i = 0; i < size; i++){
            if(key == records[i].codigo)
                return records[i];
        }
        Record vacio;
        return vacio;
    }
};

 
//" IsamFile {{{1
class IsamFile {
//" private {{{2
private:
    const string fileName = "Data/isam.dat";
    // -- Declarar los indices (crecen hasta 3 veces) --
    const string indexName = "index.dat";
    const string index2Name = "index2.dat";
    const string index3Name = "index3.dat";
    // -- Declarar factor de bloque --

    //" public {{{2
public:
    IsamFile();
    void add(Record record);
    Record search(const int& key);
    vector<Record> rangeSearch(int begin, int end);
};

//" add {{{1
// O(n/BLOCK_SIZE): Reduce los accesos a memoria llenando blockes, no unidades.
void IsamFile::add(Record record){
    fstream file;
    file.open(fileName, fstream::out | fstream::binary);
    file.seekg(0,ios::end);
    long position = file.tellg();
    int key = record.codigo;
    file.write((char*)& record, sizeof(record));
    file.close();
    Bucket index(indexName);
    Bucket index2(index2Name, index.findNextBucket(record.codigo));
    Bucket index3(index3Name, index2.findNextBucket(record.codigo));
    DataPage data(fileName, index3.findNextBucket(record.codigo));

}
//" search {{{1
// O(lgn/BLOCK_SIZE): Busqueda binaria + técnica de páginacion por bloques.
Record IsamFile::search(const int& key){
    Bucket index(indexName);
    Bucket index2(index2Name, index.findNextBucket(key));
    Bucket index3(index3Name, index2.findNextBucket(key));
    DataPage data(fileName, index3.findNextBucket(key));
    return data.bringRecord(key);
}

//" rangeSearch {{{1
// O(lgn/BLOCK_SIZE + n): Búsqueda individual + más una cantidad arbitraria de registros
// Note que el número de overflow pages podría ser incluso mayor que n.
// En ese caso extremo, este algoritmo es tan poco eficiente como una búsqueda lineal.
vector<Record> IsamFile::rangeSearch(int begin, int end){

}
