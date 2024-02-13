#include <iostream>
#include <fstream>

#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
    unsigned char primary[4];
    unsigned char secondary[4];

    int checksum = 0;
    int index = 0;
    unsigned char bytes[4];

    wchar_t* path = argv[1];

    std::fstream hive(path, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!hive) {
        std::wcout << L"Failed to open the file, aborting..." << std::endl;
        return 1;
    }
   
    hive.seekg(4, std::ios::beg);
    hive.read((char*) &primary, 4);
    hive.read((char*) &secondary, 4);

    if (primary[0] != secondary[0]) {
        std::wcout << L"Primary and second sequence number mismatch!" << std::endl;
        std::wcout << L"Performing repairs..." << std::endl;
        
        // Increment the primary sequence number by 1
        int updatedPrimary = primary[0];
        updatedPrimary++;

        // Update the primary sequence number
        hive.seekp(4, std::ios::beg);
        hive.write((char*) &updatedPrimary, 1);

        // Update the secondary sequence number
        hive.seekp(8, std::ios::beg);
        hive.write((char*) &updatedPrimary, 1);

        hive.seekg(std::ios::beg);

        // Calculate the new checksum value
        while (index <= 0x1fb) {
            hive.read((char*) &bytes, 4);

            // Convert the bytes to their little endian integer representation
            uint32_t result = (bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0]);

            checksum ^= (unsigned int) result;
            index += 0x04;
        }

        hive.seekp(0x1fc, std::ios::beg);
        hive.write((char*) &checksum, 4);
    }

    hive.close();
}
