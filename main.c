#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *takeValidIp();
int takeValidNetworksCount();
struct networkConfiguration takeConfigurationInput(int networkNo);
struct IpCidr splitIpWithCidr(const char *ip);
bool isValidIp(char ip[]);
bool isValidNetworkAddress(char ip[], char subnetMask[]);
char *calculateSubnetMask(char ip[], int cidr);
int compareNetworks(const void *a, const void *b);
struct networkConfiguration assignAddresses(struct networkConfiguration netConfig, char startIp[], char subnetMask[]);
void incrementIpAddress(char ip[], int incrementValue, char *newIp);
unsigned int calculateAddressBit(unsigned int total);

struct networkConfiguration
{
    int networkNo;
    int hostNeed;
    char networkAddress[26];
    char subnetMask[26];
    char broadcastAddress[26];
    char hostRange[60];
};

struct IpCidr
{
    int parts[4];
    int cidr;
};

int main()
{
    int networkCount;

    // Take user input
    char *ip = takeValidIp();
    printf("IP entered: %s\n", ip);
    // how many networks you need ?:
    networkCount = takeValidNetworksCount();
    // for each network, how many hosts you need ?
    struct networkConfiguration networks[networkCount];
    for (int i = 1; i <= networkCount; i++)
    {
        networks[i - 1] = takeConfigurationInput(i);
    }

    char *subnetMask = calculateSubnetMask(ip, -1);
    // Check IP is network IP
    if (!isValidNetworkAddress(ip, subnetMask))
    {
        printf("Provided ip is not valid network address.");
        return 1;
    }

    // sorting networks in desc order by host need
    qsort(networks, networkCount, sizeof(struct networkConfiguration), compareNetworks);

    char *startIp = ip;
    // For each network
    for (int i = 1; i <= networkCount; i++)
    {
        struct networkConfiguration netConfig = networks[i - 1];
        netConfig = assignAddresses(netConfig, startIp, subnetMask);
        networks[i - 1] = netConfig;
        incrementIpAddress(netConfig.broadcastAddress, 1, startIp);
    }
    for (int i = 1; i <= networkCount; i++)
    {
        printf("Network %d:\n", networks[i - 1].networkNo);
        printf("  Network Address: %s\n", networks[i - 1].networkAddress);
        printf("  Subnet Mask: %s\n", networks[i - 1].subnetMask);
        printf("  Broadcast Address: %s\n", networks[i - 1].broadcastAddress);
        printf("  Host Range: %s\n", networks[i - 1].hostRange);
    }

    return 0;
}

void incrementIpAddress(char ip[], int incrementValue, char *newIp)
{
    struct IpCidr ipCidr = splitIpWithCidr(ip);

    ipCidr.parts[3] += incrementValue;

    for (int i = 3; i > 0; i--)
    {
        if (ipCidr.parts[i] > 255)
        {
            int carry = ipCidr.parts[i] / 256;
            ipCidr.parts[i] %= 256;
            ipCidr.parts[i - 1] += carry;
        }
    }

    sprintf(newIp, "%d.%d.%d.%d",
            ipCidr.parts[0],
            ipCidr.parts[1],
            ipCidr.parts[2],
            ipCidr.parts[3]);
}

struct networkConfiguration assignAddresses(struct networkConfiguration netConfig, char startIp[], char subnetMask[])
{
    struct IpCidr ipCidr = splitIpWithCidr(startIp);

    // calcluate cidr
    int needHost = netConfig.hostNeed + 2;
    unsigned int addressBit = calculateAddressBit(needHost);
    unsigned int cidr = 32 - addressBit;
    int totalAddress = pow(2, addressBit);
    
    // network address
    char networkAddress[16];
    sprintf(networkAddress, "%d.%d.%d.%d/%d",
            ipCidr.parts[0],
            ipCidr.parts[1],
            ipCidr.parts[2],
            ipCidr.parts[3],
            cidr);
    strcpy(netConfig.networkAddress, networkAddress);

    // calculate subnet mask
    char *subnetmask = calculateSubnetMask(networkAddress, cidr);
    strcpy(netConfig.subnetMask, subnetmask);

    // calculate broadcast address
    char broadcastAddress[16];
    incrementIpAddress(netConfig.networkAddress, totalAddress - 1, broadcastAddress);
    strcpy(netConfig.broadcastAddress, broadcastAddress);

    // calculate host range
    static char hostRange[50];
    char firstHostAddress[16];
    char lastHostAddress[16];
    incrementIpAddress(netConfig.networkAddress, 1, firstHostAddress);
    incrementIpAddress(netConfig.networkAddress, totalAddress - 1 - 1, lastHostAddress);
    sprintf(hostRange, "%s - %s", firstHostAddress, lastHostAddress);
    strcpy(netConfig.hostRange, hostRange);

    return netConfig;
}

unsigned int calculateAddressBit(unsigned int total)
{
    unsigned int p = 1;
    int decreasingBit = 0;

    while (p < total)
    {
        p = p * 2;
        decreasingBit++;
    }

    return decreasingBit;
}

int compareNetworks(const void *a, const void *b)
{
    struct networkConfiguration *net1 = (struct networkConfiguration *)a;
    struct networkConfiguration *net2 = (struct networkConfiguration *)b;
    return net2->hostNeed - net1->hostNeed;
}

bool isValidNetworkAddress(char ip[], char subnetMask[])
{
    struct IpCidr ipCidr = splitIpWithCidr(ip);           // get ip parts
    struct IpCidr maskCidr = splitIpWithCidr(subnetMask); // get subnet mask parts

    if (ipCidr.cidr == -1)
    {
        return false;
    }
    for (int i = 0; i < 4; i++)
    {
        int networkPart = ipCidr.parts[i] & maskCidr.parts[i];

        if (networkPart != ipCidr.parts[i])
            return false;
    }

    return true;
}

char *calculateSubnetMask(char ip[], int cidr)
{
    if (cidr < 0 || cidr > 32)
    {
        // Get cidr from ip
        struct IpCidr ipCidr = splitIpWithCidr(ip);
        cidr = ipCidr.cidr;
    }

    static char subnetMask[15];

    // create 32 bit binary mask
    char binaryMask[32];
    for (int i = 0; i < 32; i++)
    {
        if (i < cidr)
        {
            binaryMask[i] = '1';
        }
        else
        {
            binaryMask[i] = '0';
        }
    }

    // uterate 4 parts and make decimal values
    int parts[4] = {0, 0, 0, 0};
    int partIndex = 0;
    int startIndex = 0;
    for (int i = 8; i <= 32; i += 8)
    {
        char _bitsPart[8];
        int j = 0;
        while (startIndex < i)
        {
            _bitsPart[j] = binaryMask[startIndex];
            j++;
            startIndex++;
        }
        char *endptr;
        long decimal_integer;
        // Convert the binary string to a long integer
        decimal_integer = strtol(_bitsPart, &endptr, 2);
        startIndex = i;
        parts[partIndex] = (int)decimal_integer;
        partIndex++;
    }

    // make subnet mask string
    sprintf(subnetMask, "%d.%d.%d.%d", parts[0], parts[1], parts[2], parts[3]);

    return subnetMask;
}

int takeValidNetworksCount()
{
    int count;
    while (true)
    {
        printf("how many networks you need ? ");
        scanf("%d", &count);
        if (count > 0)
            break;
        printf("please give a valid network count\n");
    }
    return count;
}

char *takeValidIp()
{
    static char ip[26];
    while (true)
    {
        printf("what is the IP address with cidr: ");
        scanf("%25s", ip);
        if (isValidIp(ip))
            break;
        printf("please give valid ip addresss.\n");
    }
    return ip;
}

struct networkConfiguration takeConfigurationInput(int networkNo)
{
    struct networkConfiguration config;
    config.networkNo = networkNo;
    // how many hosts you need ?
    while (true)
    {
        printf("how many hosts you need for network %d: ", networkNo);
        scanf("%d", &config.hostNeed);
        if (config.hostNeed > 0)
            break;
        printf("please give a valid host\n");
    }
    return config;
}

struct IpCidr splitIpWithCidr(const char *ip)
{
    struct IpCidr result = {{0, 0, 0, 0}, -1}; // default empty

    if (!ip)
        return result;

    int count = 0;
    int cidr = -1;

    // read IP with cird
    count = sscanf(ip, "%d.%d.%d.%d/%d",
                   &result.parts[0],
                   &result.parts[1],
                   &result.parts[2],
                   &result.parts[3],
                   &cidr);

    if (count < 4)
        return result; // invalid IP

    // Validate IP parts
    for (int i = 0; i < 4; i++)
    {
        if (result.parts[i] < 0 || result.parts[i] > 255)
        {
            result.cidr = -1; // mark invalid
            return result;
        }
    }

    // If cidr provided, validate it
    if (count == 5)
    {
        if (cidr < 0 || cidr > 32)
        {
            result.cidr = -1; // mark invalid
            return result;
        }
        result.cidr = cidr;
    }

    return result;
}

bool isValidIp(char ip[])
{
    struct IpCidr ipCidr = splitIpWithCidr(ip);

    if (ipCidr.cidr == -1)
    {
        return false;
    }

    if (ipCidr.parts[0] < 0 || ipCidr.parts[0] > 255 ||
        ipCidr.parts[1] < 0 || ipCidr.parts[1] > 255 ||
        ipCidr.parts[2] < 0 || ipCidr.parts[2] > 255 ||
        ipCidr.parts[3] < 0 || ipCidr.parts[3] > 255)
    {
        return false;
    }
    return true;
}
