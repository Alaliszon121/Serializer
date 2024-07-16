# Serializer

## Opis

Ten dokument opisuje sposób użycia serializera do zapisu i odczytu danych z pliku w formacie binarnym. Serializer obsługuje różne wersje danych oraz umożliwia serializację i deserializację podstawowych typów danych, jak również struktur złożonych.

## Struktura kodu

### Definicje i Makra

Serializer korzysta z kilku makr, które ułatwiają zarządzanie wersjami i serializacją poszczególnych pól:

- **`SER_VER(_version)`**: Ustawia wersję serializacji. Makro zapisuje aktualną wersję i ustawia nową wersję dla serializera.
- **`SER_END()`**: Przywraca poprzednią wersję serializacji po zakończeniu serializacji.
- **`SER_ADD(_version, _prop)`**: Serializuje pole `_prop` jeżeli wersja serializera jest większa lub równa `_version`.
- **`SER_REM(_versionAdded, _versionRemoved, _type, _prop, _defaultValue)`**: Serializuje pole `_prop` typu `_type` jeżeli wersja serializera jest większa lub równa `_versionAdded` i mniejsza niż `_versionRemoved`.

### Struktura Serializer

```cpp
struct Serializer
{
    FILE* file = nullptr;
    int version = 0;
    bool isWriting = false;
};
```

### Szablon Funkcji `serialize`

Funkcja szablonowa do serializacji podstawowych typów danych.

```cpp
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
```

### Przykład Użycia

#### Serializacja `MyData`

Struktura `MyData` zawiera pola `x` i `flag`.

```cpp
struct MyData
{
    float x;
    bool flag;
};
```

Serializacja `MyData` z obsługą wersji:

```cpp
void serialize(Serializer& ser, MyData& data)
{
    SER_VER(2);

    SER_ADD(1, data.x);
    SER_REM(1, 2, float, y, 0);
    SER_ADD(1, data.flag);

    SER_END();
}
```

#### Serializacja `MyUltraData`

Struktura `MyUltraData` zawiera pola `test`, `data` (typu `MyData`) oraz `value`.

```cpp
struct MyUltraData
{
    int test;
    MyData data;
    float value;
};
```

Serializacja `MyUltraData` z obsługą wersji:

```cpp
void serialize(Serializer& ser, MyUltraData& data)
{
    SER_VER(0);

    SER_ADD(0, data.test);
    SER_ADD(0, data.data);
    SER_ADD(0, data.value);

    SER_END();
}
```

### Funkcja `main`

Przykład użycia serializera do zapisu i odczytu danych z pliku `data.dat`.

```cpp
int main()
{
    MyUltraData data;
    data.test = 12;
    data.data.x = 42.2f;
    data.data.flag = true;
    data.value = 54.3f;

    MyUltraData dataRead = {0};

    // Serializacja do pliku
    {
        Serializer ser;
        ser.file = fopen("data.dat", "w");
        ser.isWriting = true;
        ser.version = 0;

        serialize(ser, data);

        fclose(ser.file);
    }

    // Deserializacja z pliku
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
```

## Zakończenie

Ten dokument przedstawia sposób użycia serializera w języku C++ do zapisu i odczytu danych z pliku. Dzięki obsłudze wersji, serializer umożliwia elastyczne zarządzanie strukturą danych, co jest szczególnie przydatne w aplikacjach, gdzie format danych może ulegać zmianie w kolejnych wersjach programu.
