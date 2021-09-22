#include <fstream>
#include <iostream>
#include "lib.hpp"
#include <limits>
#include <string>
#define TAM_PAGE 4096


constexpr int M = (TAM_PAGE - sizeof(long))/(sizeof(long) + sizeof(int));
constexpr int K = (TAM_PAGE - sizeof(long) - sizeof(int))/sizeof(Record);

int closeBinarySearch(int arr[], int l, int r ,int x){
    if(r > l && r-l != 1){
        int mid = l+(r-l)/2;
        if(arr[mid] == x){
            return mid;
        }
        if(arr[mid] > x)
            return closeBinarySearch(arr, l, mid -1 , x);
        return closeBinarySearch(arr, mid + l, r, x);
    }
    if(r == l)    
        return(l<x)? l+1:l;
    return -1;
    
}


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

int main(){
    /*std::fstream index;
    index.open("index.dat", std::fstream::out | std::fstream::binary);
    int arrk[] ={2,10,39,60, 89, 330, 29929, 3982094};
    long arrp[] ={0,0,1,1,2,2,3,3,4};
    for(int i = 0; i < 17; i++ ){
        if(i%2==0)
            index.write((char*)&arrp[i/2],sizeof(long));
        else
            index.write((char*)&arrk[i/2], sizeof(int));
    }
    index.close();*/
    Bucket n;
    n.load("index.dat");
    std::cout<<n;
    std::cout<<n.findNextPage(11)<<std::endl;

    return 0;
}
