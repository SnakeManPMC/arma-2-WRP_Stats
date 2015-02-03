#include <QtCore/QCoreApplication>
#include <cstdio>
#include <cstdlib>
#include "wrpstats.h"

wrpStats::wrpStats(int argc, char *argv[])
{
    Open_Files(argc, argv);
    Read_Signature();

    // 8WVR, ArmA style
    if (strcmp (sig, "8WVR") == 0) Read_8WVR();

    // 4WVR, OFP style
    if (strcmp (sig, "4WVR") == 0) Read_4WVR();
}


void wrpStats::Open_Files(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Error in parameters, use:\nWRP_Stats.exe WRP_FILE\n");
        exit(1);
    }

    map = fopen(argv[1], "rb");
    if (!map)
    {
        printf("error in %s\n", argv[1]);
        exit(1);
    }

    printf ("Opened: %s\n", argv[1]);
}


void wrpStats::Read_Signature()
{
    fread(sig, 4, 1, map);
    sig[4] = 0;
    printf("WRP Signature: %s\n", sig);
}


void wrpStats::Read_8WVR()
{
    fread(&texturegrid, 4, 1, map);
    printf("texture grid x: %d\n", texturegrid);
    fread(&texturegrid, 4, 1, map);
    printf("texture grid z: %d\n", texturegrid);

    fread(&terraingrid, 4, 1, map);
    printf("terrain grid x: %d\n", terraingrid);
    fread(&terraingrid, 4, 1, map);
    printf("terrain grid z: %d\n", terraingrid);

    // cell size, but how many digits??
    fread(&cellsize, 4, 1, map);
    printf("Cellsize: %f\n", cellsize);

    // reading elevations
    for (int i = 0; i < terraingrid * terraingrid; i++)
    {
        fread(&elevation, 4, 1, map);
        //printf("Elevation: %f\n", elevation);
    }

    // credits go to Mikero for helping me figure out the RVMAT parts
    // reading rvmat index
    for (int i = 0; i < texturegrid * texturegrid; i++)
    {
        fread(&materialindex, 2, 1, map);
//		printf("materialindex: %d\n", materialindex);
    }

    // noofmaterials
    fread(&noofmaterials, 4, 1, map);
    printf("noofmaterials: %ld\n", noofmaterials);

    // reading the first NULL material...
    fread(&materiallenght, 4, 1, map);
    printf("1st NULL materiallenght: %d\n", materiallenght);

    //////////// now, insert  the following
    noofmaterials--; // remove that 1st one. from the count

    while (noofmaterials--) //read the rest (if any)
    {
        fread(&materiallenght, 4, 1, map);
        if (!materiallenght)
        {
            printf("%ld materiallenght has no count\n", noofmaterials);
            exit(1);
        }
        fread(&buffer, materiallenght, 1, map);
        buffer[materiallenght]=0; // make it asciiz
        fread(&materiallenght, 4, 1, map);
        if (materiallenght)
        {
            printf("%ld 2nd materiallenght should be zero\n", noofmaterials);
            exit(1);
        }
    }

    // Start reading objects...

    printf ("Reading 3d objects...");

    for(;;)
    {
        fread(&dDir, sizeof(float), 3*4, map);
        fread(&dObjIndex, sizeof(long), 1, map);
        fread(&len, sizeof(long), 1, map);

        if (!len) break;// last object has no name associated with it, and the transfrom is garbage

        fread(dObjName,sizeof(char),len,map);
    }
    // should now be at eof
    printf(" Done\nNumber of P3D objects: %ld\n", dObjIndex);

    printf ("All fine, 8WVR file read, exiting. Have a nice day!\n");
}


void wrpStats::Read_4WVR()
{
    fread(&texturegrid, 4, 1, map);
    printf("Cells: %d\n", texturegrid);
    fread(&texturegrid, 4, 1, map);
//	printf("texture grid z: %d\n", texturegrid);

    // reading elevations
    for (int i = 0; i < texturegrid * texturegrid; i++)
    {
        // this was elevation before (float), but it supposed to be short??
        fread(&materialindex, sizeof(short), 1, map);
    }

    printf("Reading texture indexes...");

    // read textures indexes
    for (int i = 0; i < texturegrid * texturegrid; i++)
    {
        fread(&materialindex, sizeof(short), 1, map);
    }

    printf(" Done\nReading texture names...");

    // textures 32 char length and total of 512
    for (int ix = 0; ix < 512; ix++)
    {
        sig[0] = 0;
        fread(sig, 32, 1, map);
    }

    printf(" Done\nReading 3d objects...");

    while (!feof(map))
    {
        fread(&dDir, sizeof(float), 3*4, map);
        fread(&dObjIndex, sizeof(long), 1, map);
        fread(dObjName, 76, 1, map);
        //dObjName[76] = 0; // asciiz
    }
    // should now be at eof
    printf(" Done\nNumber of P3D objects: %ld\n", dObjIndex);

    printf ("All fine, 4WVR file read, exiting. Have a nice day!\n");
}


void wrpStats::Close_Files()
{
    fclose(map);
}
