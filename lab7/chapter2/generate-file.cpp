#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdint>
#include "file-struct.h"

using namespace std;

std::ofstream& operator<< (std::ofstream& out, struct record_s& data)
{
    out.write(reinterpret_cast<char*>(&data), sizeof(record_s));
    return out;
}

/*
std::ifstream& operator>> (std::ifstream& in, struct record_s& data)
{
    in.read(reinterpret_cast<char*>(&data), sizeof(record_s)); 
    return in;
}
*/

void writeRecord(struct record_s record, ofstream& out)
{
    out << record;
}

int main()
{
    struct record_s f;
    cin >> f.name;
    cin >> f.address;
    cin >> f.semester;

    ofstream fout("file.bin", ios::out | ios::binary);

    writeRecord(f, fout);

    fout.close();

    return 0;
}