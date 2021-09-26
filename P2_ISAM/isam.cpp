#include <fstream>
#include <iostream>
#include "lib.hpp"
#include <limits>
#include <string>
#include <vector>
#define TAM_PAGE 4096

int d = 0;
//0 1 2 3 4   5
constexpr int M = (TAM_PAGE - sizeof(long))/(sizeof(long) + sizeof(int));
constexpr int K = (TAM_PAGE - sizeof(long) - sizeof(int))/sizeof(Record);

//" closeBinarySearch {{{1
int closeBinarySearch(int arr[], int l, int r ,int x){

    if(r > l && r-l != 1){
        int mid = l+(r-l)/2;
        if(arr[mid] == x){
            return mid;
        }
        if(arr[mid] > x)
            return closeBinarySearch(arr, l, mid -1 , x);
        return closeBinarySearch(arr, mid + 1, r, x);
    }
    if(r == l)    
        return(arr[l]<x)? l+1:l;
    return -1;
    
}

//" Bucket {{{1
struct Bucket{
    int size = 0;
    int keys[M];
    long pages[M+1];
    
    Bucket(){
        for(int i = 0; i< M; i++){
            keys[i] = std::numeric_limits<int>::max();
            pages[i] = 0;
        }
        pages[M] = 0;
    
    }
    void load(std::string fileName, long position = 0){
        std::fstream file;
        file.open(fileName, std::fstream::in | std::fstream::binary);
        file.seekg(position, std::ios::beg);
        for(int i = 0; i < 2*M +1;i++){
            if(i%2 == 0){
                file.read((char*)&pages[i/2], sizeof(pages[i/2]));
            }
            else{ 
                if(file.read((char*)&keys[i/2], sizeof(keys[i/2])))
                    size++;
            }
        }
        file.close();
    }

    long findNextPage(int key){
        return pages[closeBinarySearch(keys, 0, M - 1, key)];
    }
};
//" Funciones y sobrecargas {{{1 
std::ostream & operator <<(std::ostream & os, Bucket & b){
    for(int i = 0 ; i < 2*M +1 ; i++){
        if(i%2==0){
            os<<"P"<<b.pages[i/2]<<" ";
        }
        else
            os <<"K"<< b.keys[i/2]<<" ";
    }
    os << std::endl;
    os <<"size: "<< b.size<<std::endl;
    return os;
}

void updateRecord(Record& record, long& position){
    std::fstream file;
    file.open("data.dat", std::fstream::out | std::fstream::binary);
    file.seekg(position, std::ios::beg);
    file.write((char*)&record, sizeof(record));
    file.close();
}

long appendNewRecordOnFile(Record record){
    std::fstream file;
    long last;
    file.open("data.dat", std::fstream::app | std::fstream::binary);
    file.write((char*)&record, sizeof(record));
    last = file.tellg();
    file.close();
    return last - sizeof(record);

}

void updateIndex(int key, long position){
    std::fstream file;
    file.open("index1.dat", std::fstream::app | std::fstream::binary);
    file.write((char*)& key, sizeof(key));
    file.write((char*)& position, sizeof(position));
    file.close();
}
//" DataPage {{{1
struct DataPage{

    Record records[K];
    int size;
    long nextPage = -1;

    DataPage(){
        size = 0;
    }
    
    void load(std::string fileName="data.dat", long position=0){
        std::fstream file;
        long positionToLoad = position;
        file.open(fileName, std::fstream::in | std::fstream::binary);
        while(positionToLoad != -1){
            if(size < K){
                file.seekg(positionToLoad, std::ios::beg);
                file.read((char*)& records[size], sizeof(records[size]));
                positionToLoad = records[size].nextRecord;
                size++;
            }
            if(size == K){
            nextPage = positionToLoad;
            positionToLoad = -1;
            }
        }
        file.close();
    }

    Record linearSearch(int key){
        if(nextPage != -1 && key > records[K-1].codigo){
            DataPage temp;
            temp.load("data.dat", nextPage);
            return temp.linearSearch(key);
        }
        for(int i = 0;i < size; i++){
            if(records[i].codigo == key)
                return records[i];
        }
        return {0,0,0,0,-2};
    }

    void tryAddRecord(Record& record){
        if(size==0){
            long longius;
            longius = appendNewRecordOnFile(record);
            records[0] = record;
            updateIndex(record.codigo, longius);
            size++;
        }
        if(size < K){
            int i;
            for(i = 0;records[i].codigo <= record.codigo && i < size ;i++);
            record.nextRecord = records[i].nextRecord;
            records[i].nextRecord = appendNewRecordOnFile(record);
            updateRecord(records[i], records[i-1].nextRecord);
            records[size] = record;
            size++;
        }
        if(size == K){

            DataPage nueva;
            long next;
            nueva.load("data.dat", next = appendNewRecordOnFile(record));
            nextPage = next;
        }
    }
};

//" Isam {{{ 1
class Isam{
    public:
        Isam(std::string fileName){
            this->fileName = fileName;
            root = 0;
        }
        void add(Record record){
            DataPage temp;
            temp.load(fileName, searchFromRoot(record.codigo));
            temp.tryAddRecord(record);
        }
        Record search(int key){
            DataPage temp;
            temp.load(fileName,searchFromRoot(key));
            return temp.linearSearch(key);
        }
        std::vector<Record> rangeSearch(int start, int end){
           // [4 93 1001]23903809280 
            std::vector<Record> result;
           return result; 
        }

    private:
        int root;
        std::string fileName;
        std::string indexFile[3] = {"index1.dat", "index2.dat", "index3.dat"}; 
        long searchFromRoot(int key){
            long position = 0;
            for(int i = 0 ; i<= root; i++){
                Bucket index;
                index.load(indexFile[i],position);
                position =  index.findNextPage(key);
            }
            return position;
        }
        
};