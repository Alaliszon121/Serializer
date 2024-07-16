#include <iostream>
#include <stdio.h>

/*
    FILE* file = fopen("text.txt", "w");
    if(!file)
    {
        printf("File not found");
        return 1;
    }

    void* buffer = nullptr;
    size_t size = 0;

    fread(buffer, size, 1, file);
    size_t pos = ftell(file);

    size_t offset = 0;
    fseek(file, offset, SEEK_SET);    // SEEK_SET - beginning of file, SEEK_CUR - current position, SEEK_END - end of file

    // get file size
    size_t cur = ftell(file);
    fseek(file, 0, SEEK_END);   // go to end of file
    size_t size = ftell(file);
    fseek(file, cur, SEEK_SET);

    fwrite(buffer, size, 1, file);
*/

/*

{
        FILE* file = fopen("text.txt", "w");
        if(!file)
        {
            printf("File not found");
            return 1;
        }

        const char* txt = "hello file!";
        size_t size = strlen(txt);

        fwrite(txt, size, 1, file);

        fclose(file);
    }

    {
        FILE* file = fopen("text.txt", "r");
        if (!file)
        {
            printf("File not found");
            return 1;
        }

        fseek(file, 0, SEEK_END);   // go to end of file
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char txt[128] = {0};
        fread(txt, size, 1, file);

        fclose(file);
        printf("File content: %s\n", txt);
    }

*/

/*
float data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float dataRead[4] = {0.0f};

    FILE* fileWrite = fopen("text2.txt", "w");
    if (!fileWrite) {
        printf("File not found!\n");
        return 1;
    }

    fwrite(data, sizeof(float), 4, fileWrite);
    fclose(fileWrite);

    FILE* fileRead = fopen("text2.txt", "r");
    if (!fileRead) {
        printf("File not found!\n");
        return 1;
    }

    fread(dataRead, sizeof(float), 4, fileRead);
    fclose(fileRead);

    printf("dataRead: %1.1f, %1.1f, %1.1f, %1.1f\n", dataRead[0], dataRead[1], dataRead[2], dataRead[3]);
*/

/*
struct MyData
    {
        float x;
        short value;
        int z;
    };

    MyData dataRead {};

    FILE* fileRead = fopen("text3.txt", "r");
    if (!fileRead) {
        printf("File not found!\n");
        return 1;
    }

    fread(&dataRead, sizeof(MyData), 1, fileRead);
    fclose(fileRead);

    printf("Read data: x=%1.1f, y=%i, z=%i\n", dataRead.x, dataRead.value, dataRead.z);
*/

#pragma warning(disable:4996)

/* SERIALIZATION */
struct Serializer
{
    FILE* file = nullptr;
    int version = 0;
    bool isWriting = false;
};

template<typename T>
void serialize(Serializer& ser, T& prop)
{
    static_assert(std::is_fundamental<T>::value, "Type is not fundamental!");

    if (ser.isWriting) {
        fwrite(&prop, sizeof(T), 1, ser.file);
    }
    else {
        fread(&prop, sizeof(T), 1, ser.file);
    }
}

#define SER_VER(_version)   \
    int scopedVersion = ser.version;    \
    if(ser.isWriting)   \
    {   \
        ser.version = _version; \
    }   \
    serialize(ser, ser.version)

#define SER_END()   \
    ser.version = scopedVersion;

#define SER_ADD(_version, _prop) \
    if(ser.version >= _version) \
    {   \
        serialize(ser, _prop);  \
    }

#define SER_REM(_versionAdded, _versionRemoved, _type, _prop, _defaultValue)   \
    _type _prop = _defaultValue;    \
    if(ser.version >= _versionAdded && ser.version < _versionRemoved)   \
    {   \
        serialize(ser, _prop);  \
    }
/* SERIALIZATION */

struct MyData
{
    float x;
    bool flag;
};

void serialize(Serializer& ser, MyData& data)
{
    SER_VER(2);
    
    SER_ADD(1, data.x);
    SER_REM(1, 2, float, y, 0);
    SER_ADD(1, data.flag);

    SER_END();
}

struct MyUltraData
{
    int test;
    MyData data;
    float value;
};

void serialize(Serializer& ser, MyUltraData& data)
{
    SER_VER(0);

    SER_ADD(0, data.test);
    SER_ADD(0, data.data);
    SER_ADD(0, data.value);

    SER_END();
}

int main()
{
    MyUltraData data;
    data.test = 12;
    data.data.x = 42.2f;
    data.data.flag = true;
    data.value = 54.3f;

    MyUltraData dataRead = {0};

    // {
    //     Serializer ser;
    //     ser.file = fopen("data.dat", "w");
    //     ser.isWriting = true;
    //     ser.version = 0;

    //     serialize(ser, data);

    //     fclose(ser.file);
    // }

    {
        Serializer ser;
        ser.file = fopen("data.dat", "r");
        ser.isWriting = false;
        ser.version = 0;

        serialize(ser, dataRead);

        fclose(ser.file);

        printf("Content: %i, %1.1f, %i, %1.1f\n", dataRead.test, dataRead.data.x, dataRead.data.flag, dataRead.value);
    }

    return 0;
}
