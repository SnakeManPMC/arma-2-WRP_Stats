#ifndef WRPSTATS_H
#define WRPSTATS_H

class wrpStats
{
public:
    wrpStats(int argc, char *argv[]);
    void Open_Files(int argc, char *argv[]);
    void Read_Signature();
    void Read_8WVR();
    void Read_4WVR();
    void Close_Files();

private:
    FILE *map;
    char buffer[2000];
    char dObjName[2000];
    char sig[33];
    float cellsize, elevation, dDir[3][4];
    int len, texturegrid, terraingrid;
    long noofmaterials;
    short materialindex;
    ulong dObjIndex, materiallenght;
};

#endif // WRPSTATS_H
