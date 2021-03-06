#include "seqfile.cpp"

int seqFileTest(){
    SequentialFile SF = SequentialFile();
    vector<SeqFile::Record> testData {};
    for(const auto& record: SeqFile::autoGeneratedDataset) testData.push_back(record);
    sort(testData.begin(), testData.end(), SeqFile::recordComp);
    SF.insertAll(testData); //Revisar lib.hpp
    
    int errCount = 0;
    cout << "\t\t\t ==== P1 SEQUENTIAL FILE ===\n";

    cout << "Probando insertAll... \n";
    ifstream file("Data/seqfile.dat", ios::binary);
    
    file.seekg(0, ios::beg);
    SeqFile::Record record;
    auto testIdx = 0;
    while(file.read((char*) &record, sizeof(SeqFile::Record))){
        if(record.codigo == testData[testIdx++].codigo) continue;
        else cout << "ERROR (" << ++errCount << "): Hubo un problema con el insertAll.\n";
    }
    if(errCount == 0) cout <<  "PASSED: insertAll esta ok.\n";
    
    cout << "Probando search... \n";
    for(const auto& record: testData){
        if(SF.search(record.codigo).codigo == record.codigo) continue;
        else cout << "ERROR (" << SF.search(record.codigo).codigo << " - " << record.codigo << "): Hubo un problema con el search.\n";
    }
    if(errCount == 0) cout <<  "PASSED: search esta ok.\n";
    
    cout << "Probando rangeSearch... \n";
    vector<SeqFile::Record> someRecords = SF.rangeSearch(0, 30);
    testIdx = 0;
    for(const auto& record: someRecords){
        if(record.codigo == testData[testIdx++].codigo) continue;
        else cout << "ERROR (" << ++errCount << "): Hubo un problema con el rangeSearch.\n";
    }
    if(errCount == 0) cout <<  "PASSED: rangeSearch esta ok.\n";
    
    cout << "Añadiendo registros... \n";
    SeqFile::Record newRecords [4] = {
        {55,"Ralina","Derecho",10,0},       // Testeando: Insercion estándar
        {52,"Hollyanne","CS",2,0},          // Testeando: Insercion con next en heap
        {5,"SuzanWn","Ing. Quimica",3,0},   // Testeando: Insercion al comienzo
        {7,"Alexia","CS",10,0}              // Testeando: Insercion entre el menor elemento e index 0
    };
    for (const auto& record: newRecords) {
        SF.add(record);
        testData.push_back(record);
    }
    sort(testData.begin(), testData.end(), SeqFile::recordComp);

    cout << "Probando search (luego de add)... \n";
    for(const auto& record: testData){
        if(SF.search(record.codigo).codigo == record.codigo) continue;
        else cout << "ERROR (" << SF.search(record.codigo).codigo << " - " << record.codigo << "): Hubo un problema con el add o el search.\n";
    }
    if(errCount == 0) cout <<  "PASSED: search (luego de add) esta ok.\n";

    cout << "Probando rangeSearch (luego de add)... \n";
    someRecords = SF.rangeSearch(0, 34);
    testIdx = 0;
    for(const auto& record: someRecords){
        if(SF.search(record.codigo).codigo == record.codigo) continue;
        else cout << "ERROR (" << ++errCount << "): Hubo un problema con el add o el rangeSearch.\n";
    }
    if(errCount == 0) cout <<  "PASSED: rangeSearch (luego de add) esta ok.\n"; 
    
    cout << "Forzando reset... \n";
    SeqFile::Record overflow = {37,"Heider","CS",1,0};
    SF.add(overflow);
    
    if (errCount == 0) cout<< "PASSED ALL TESTS.\n";
    file.close();
    return 0;
}