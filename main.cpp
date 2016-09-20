// --------------------------------------------------------------------------------------------------------------
// main.cpp
//
// Copyright (C) 2016 par GeniALE.
// Marc-Andre Guimond <guimond.marcandre@gmail.com>.
// Tous droits reserves.
//
// Ce fichier est encode en UTF-8.
// --------------------------------------------------------------------------------------------------------------

// Standard includes.
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cstdlib>
#include <errno.h>

// App includes.
#include "BBConfig.h"
#include "BBNetworkDiscovery.h"

// Lib includes.
#include "UDPClient.h"

// Common includes.
#include "Host.h"
#include "version.h"
#include "utils.h"

using namespace std;

// --------------------------------------------------------------------------------------------------------------
// Private constants.

// Location ID offset
#define kLocationIDOffset 17

// Subnet.
#define kSubnet "192.168.2."

// Max hosts to discover.
#define kNetworkDevicesMaxHosts 5

// BeagleBrew UDP traffic port.
#define kUDPPortBB 34923

// Config file parsing tables.
static NameID_t gLocationTable[] =  kConfigDeviceMapping_LocationIDTable;

// --------------------------------------------------------------------------------------------------------------
// Private data types.

typedef struct
{
    unsigned int locationID;
    const char* name;
} LocationIDTable_t;

// --------------------------------------------------------------------------------------------------------------
// Private variables.

int gDiscoveryList[kNetworkDevicesMaxHosts];

// --------------------------------------------------------------------------------------------------------------
static void PrintHelp(char* inArg0)
{
    printf("Usage:\n"
           "\t%s [option]\n"
           "Options:\n"
           "\t-ver\t\tGet version info.\n"
           "\t-vcs\t\tGet VCS info.\n"
           "\t-t\t\tNumber of discovery scan iterations.\n", inArg0);
}

// --------------------------------------------------------------------------------------------------------------
static bool BBNetCheckDiscoveryList(int inLocationID)
{
    bool isNotInList = true;

    for (uint8_t deviceIdx = 0; deviceIdx < kNetworkDevicesMaxHosts; deviceIdx ++)
    {
        if (gDiscoveryList[deviceIdx] == inLocationID)
        {
            // Device already in list.
            isNotInList = false;
            break;
        }
    }

    return isNotInList;
}

// --------------------------------------------------------------------------------------------------------------
static bool BBNetAddDeviceToList(int inDeviceID)
{
    static int deviceListIdx = 0;
    bool isSuccess = false;

    if (BBNetCheckDiscoveryList(inDeviceID))
    {
        gDiscoveryList[deviceListIdx ++] = inDeviceID;
        isSuccess = true;
    }
    else
    {
        isSuccess = false;
    }

    return isSuccess;
}

// ----------------------------------------------------------------------------
static const char* DeviceLocationGetName(unsigned int inID)
{
    NameID_t* inNameIDTable = gLocationTable;

    for (; inNameIDTable->ID != kDeviceLocationID_Unknown; inNameIDTable ++)
    {
        if (inNameIDTable->ID == inID)
        {
            break;
        }
    }

    return inNameIDTable->name;
}

// --------------------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        PrintHelp(argv[0]);
    }
    else
    {
        if (strncmp("-ver", argv[1], strlen("-ver")) == 0)
        {
            printf("%s\n", versionID);
            exit(1);
        }
        else if (strncmp("-vcs", argv[1], strlen("-vcs")) == 0)
        {
            printf("%s\n", versionVCS);
            exit(1);
        }
        else if (strncmp("-t", argv[1], strlen("-t")) == 0)
        {
            if (argc < 3)
            {
                PrintHelp(argv[0]);
                exit(-1);
            }

            int socketFileDescriptor = UDPBindSocket(kUDPPortBB);
            if (socketFileDescriptor < 0)
            {
                PrintMessage(__FILENAME__, __FUNCTION__, "Error", "Connect failed (%s)", strerror(errno));
                exit(-1);
            }

            int status = kSuccess;
            int discoveryMaxIterations = atoi(argv[2]);
            for (int discoveryIterations = 0; discoveryIterations < discoveryMaxIterations; discoveryIterations ++)
            {
                uint8_t* udpBuffer = (uint8_t*)calloc(1, kUDPBufferSize);
                int serverPort;
                char networkAddress[kHostAddressSize] = { 0 };

                if ((status = UDPGetServerInfo(socketFileDescriptor, &serverPort, networkAddress, udpBuffer)) < 0)
                {
                    PrintMessage(__FILENAME__, __FUNCTION__, "Error", "Failed to read UDP packet (%s)", strerror(errno));
                    exit(-1);
                }
                else
                {
                    char* hostAddress = networkAddress + strlen(kSubnet);
                    if (BBNetAddDeviceToList(atoi(hostAddress)))
                    {
                        printf("Found %s on port %d ---> %s\n", networkAddress, serverPort, DeviceLocationGetName(udpBuffer[kLocationIDOffset]));
                    }
                }

                free(udpBuffer);
            }

            printf("Discovery terminated.\n");

            status = UDPCloseSocket(socketFileDescriptor);
            if (status < 0)
            {
                PrintMessage(__FILENAME__, __FUNCTION__, "Error", "Disconnect failed (%s)", strerror(errno));
                exit(-1);
            }
        }
        else
        {
            PrintHelp(argv[0]);
        }
    }

    exit(1);
}
