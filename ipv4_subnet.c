#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


// Convert a binary string to an integer
int binary_to_num(char * binary)
{
    double power;

    int decimal = 0;
    power = 7.0;
    for (int i = 0; i < 8; i++)
    {
        if (binary[i] == '1')
        {
            decimal += pow(2, power);
        }

        power--;
    }

    return decimal;
}

// Convert an integer to a binary string
char * num_to_binary(int quotient)
{
    char *binary = malloc(9);
    binary[8] = '\0';
    int num = 7;

    while (quotient > 0)
    {
        binary[num] = (quotient % 2) + '0';
        quotient = quotient / 2;
        num--;
    }

    while (num >= 0) 
    {
        binary[num] = '0';
        num--;
    }  

    return binary; 
}


// Split up a string IP into an array of integers
int * split_ip(char * ip)
{
    char currentOctet[4];
    int currentOctetNum = 0;
    int num = 0;
    int *splitIP = malloc(4 * sizeof(int));

    // Iterate through IP address
    for (int i = 0; i < strlen(ip); i++)
    {
        // When we have reached the end of an octet
        if (ip[i] == '.')
        {
            if (num == 1)
            {
                currentOctet[2] = currentOctet[0];
                currentOctet[0] = '0';
            } 
            else if (num == 2)
            {
                currentOctet[2] = currentOctet[1];
                currentOctet[1] = currentOctet[0];
                currentOctet[0] = '0';
            }

            currentOctet[3] = '\0';
            splitIP[currentOctetNum] = atoi(currentOctet);

            // Clear currentOctet

            for (int i = 0; i < 3; i++)
            {
                currentOctet[i] = '0';
            }

            // Start next octet and reset num
            currentOctetNum++;
            num = 0;
        } 
        else 
        {
            // Add current character to currentOctet
            currentOctet[num] = ip[i];
            num++;
        }
    }

    if (num == 1)
    {
        currentOctet[2] = currentOctet[0];
        currentOctet[0] = '0';
    } 
    else if (num == 2)
    {
        currentOctet[2] = currentOctet[1];
        currentOctet[1] = currentOctet[0];
        currentOctet[0] = '0';
    }

    currentOctet[3] = '\0';
    splitIP[currentOctetNum] = atoi(currentOctet);

    return splitIP;
}

int main(int argc) 
{
    char ip[256];
    char subnetMask[256];

    // Get IP address and Subnet mask from user
    printf("Enter IP: ");
    scanf("%s", ip);
    printf("Enter Subnet Mask: ");
    scanf("%s", subnetMask);

    printf("\n");

    // Print IP and subnet mask
    printf("IP: %s\n", ip);
    printf("Subnet Mask: %s\n", subnetMask);

    // Split IP address
    int *splitIP = split_ip(ip);

    // Split subnet mask
    int *splitMask = split_ip(subnetMask);

    // Convert IP from decimal to binary
    char binaryIP[4][9];

    for (int i = 0; i < 4; i++)
    {
        strcpy(binaryIP[i], num_to_binary(splitIP[i]));
    }

    // Convert subnet mask from decimal to binary
    char binaryMask[4][9];

    for (int i = 0; i < 4; i++)
    {
        strcpy(binaryMask[i], num_to_binary(splitMask[i]));
    }

    // Get prefix from subnet mask
    int prefix = 0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (binaryMask[i][j] == '1')
            {
                prefix++;
            }
        }
    }

    // Convert IP to network address
    printf("Prefix: /%i\n", prefix);

    for (int i = prefix / 8; i < 4; i++)
    {
        for (int j = prefix % 8; j < 8; j++)
        {
            binaryIP[i][j] = '0';
        }
    }

    // Convert IP from binary to decimal to show network address
    int networkIP[4];

    for (int i = 0; i < 4; i++)
    {
        networkIP[i] = binary_to_num(binaryIP[i]);
    }

    printf("Network Address: %i.%i.%i.%i\n", networkIP[0], networkIP[1], networkIP[2], networkIP[3]);

    // Find out what class this network is
    
    if (prefix == 8 || prefix == 16 || prefix == 24)
    {
        printf("Usable Host Addresses: %i\n", (int)pow(2, 32 - prefix) - 2);
        return 0;
    }

    int hostOctetStart;

    if (networkIP[0] <= 126)
    {
        hostOctetStart = 1;
    }
    else if (networkIP[0] <= 191)
    {
        hostOctetStart = 2;
    }
    else if (networkIP[0] <= 223)
    {
        hostOctetStart = 3;
    }

    // Calculate subnet bits and host bits
    int subnetBits = 0;
    int hostBits = 0;
    int finalOctet;

    for (int i = hostOctetStart; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (binaryMask[i][j] == '1')
            {
                subnetBits += 1;
                finalOctet = i;
            }
            else
            {
                hostBits += 1;
            }
        }

        if (subnetBits == 8 && hostOctetStart < 3)
        {
            subnetBits = 0;
        }
    }

    int networks = pow(2, subnetBits);
    int magicNumber = 256 / networks;
    int usableHosts = pow(2, hostBits) - 2;

    printf("Usable Host Addresses/Subnet: %i\n", usableHosts);

    int count = 256;
    int subnetAddress[4];

    for (int i = 0; i < 4; i++)
    {
        subnetAddress[i] = networkIP[i];
    }

    subnetAddress[finalOctet] = 0;

    printf("\nAll %i Possible /%i Networks For %i.%i.%i.%i\n", networks, prefix, networkIP[0], networkIP[1], networkIP[2], networkIP[3]);

    while (count != 0 && magicNumber != 0)
    {
        count -= magicNumber;
        subnetAddress[finalOctet] = count;
        printf("%i.%i.%i.%i\n", subnetAddress[0], subnetAddress[1], subnetAddress[2], subnetAddress[3]);
    }


    // Free malloced memory
    free(splitIP);

    return 0;
}