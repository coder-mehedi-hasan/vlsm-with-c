#include <stdio.h>
#include <stdbool.h>

char *takeValidIp();
int takeValidNetworksCount();
struct networkConfiguration takeConfigurationInput(int networkNo);
struct IpCidr splitIpWithCidr(const char *ip);
bool isValidIp(char ip[]);


struct networkConfiguration
{
    int networkNo;
    int hostNeed;
    char networkAddress[26];
    char subnetMask[26];
    char broadcastAddress[26];
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

    for (int i = 1; i <= networkCount; i++)
    {
        printf("Network %d needs %d hosts\n", networks[i - 1].networkNo, networks[i - 1].hostNeed);
    }

    // Check IP is network IP

    // For each network

    // calculate cidr
    // calculate subnet mask
    // calculate broadcast address

    return 0;
}

int takeValidNetworksCount(){
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
