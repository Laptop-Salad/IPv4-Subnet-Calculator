#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int display_subnets(int magicNumber, int octet, int address[])
{
    /*
      Increase the last octet with a subnet bit by the magic number up until 256

      Example:

      Input:
      magicNumber = 128
      octet = 3
      address = [192, 168, 0, 0]

      Output:
      192.168.0.0
      192.168.0.128

    */
    int count = 0;
    while (count < 256 && magicNumber != 0)
    {
        count += magicNumber;

        if (count == 256)
        {
            break;
        }

        address[octet] = count;
        printf("%i.%i.%i.%i\n", address[0], address[1], address[2], address[3]);
    }
}

int binary_to_num(char * binary)
{
    /*
     Convert a binary string to an integer
     Example: "11000000" -> 192

     The integer equivalent is the sum of 2 to the power of each value
    */

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


char * num_to_binary(int quotient)
{
    /*
     Converts an integer to its 8-bit binary equivalent
     Example: 192 -> "11000000"

     The binary equivalent can be found by dividing the number by two until it reaches zero, the remainders
     in reverse is the binary string.

    */
    char *binary = malloc(9);

    // Adding a terminator character to be able to return a string
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


int * split_ip(char * ip)
{
    /*
     Split up a string IP into an array of integers
     Example: "192.168.0.1" -> [192, 168, 0, 1]

    */
    char currentOctet[4];
    int currentOctetNum = 0;
    int digits = 0;
    int *splitIP = malloc(4 * sizeof(int));

    // Iterate through IP address and save characters in currentOctet
    for (int i = 0; i < strlen(ip); i++)
    {
        // When we have reached the end of an octet
        if (ip[i] == '.')
        {
            /*
             Handle instances where the character(s) is less than three characters
             Example:
             Characters = 2
             currentOctet = [2, 0, 0]
             
             Result = [0, 0, 2]

             Example:
             Characters = 64
             currentOctet = [6, 4, 0]

             Result = [0, 6, 4]
            */
            if (digits == 1)
            {
                currentOctet[2] = currentOctet[0];
                currentOctet[0] = '0';
            } 
            else if (digits == 2)
            {
                currentOctet[2] = currentOctet[1];
                currentOctet[1] = currentOctet[0];
                currentOctet[0] = '0';
            }

            // Add a terminator character so that currentOctet is a string and can be converted to an integer
            currentOctet[3] = '\0';
            splitIP[currentOctetNum] = atoi(currentOctet);

            // Clear currentOctet
            for (int i = 0; i < 3; i++)
            {
                currentOctet[i] = '0';
            }

            // Start next octet and reset num
            currentOctetNum++;
            digits = 0;
        } 
        else 
        {
            // Add current character to currentOctet
            currentOctet[digits] = ip[i];
            digits++;
        }
    }

    // Handle if character(s) are less than three characters long for last octet
    if (digits == 1)
    {
        currentOctet[2] = currentOctet[0];
        currentOctet[0] = '0';
    } 
    else if (digits == 2)
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

    // Display IP and subnet mask to user
    printf("IP: %s\n", ip);
    printf("Subnet Mask: %s\n", subnetMask);

    // Split string IP address into array of four integers
    int *splitIP = split_ip(ip);

    // Split subnet mask into array of four integers
    int *splitMask = split_ip(subnetMask);

    // For each integer in splitIP convert it into a binary string
    char binaryIP[4][9];

    for (int i = 0; i < 4; i++)
    {
        strcpy(binaryIP[i], num_to_binary(splitIP[i]));
    }

    // For each integer in splitMask convert it into a bianry string
    char binaryMask[4][9];

    for (int i = 0; i < 4; i++)
    {
        strcpy(binaryMask[i], num_to_binary(splitMask[i]));
    }

    // Get prefix length from subnet mask: count amount of '1's
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

    // Convert IP to network address, by removing '1's in IP address after the host bits in the subnet mask
    printf("Prefix: /%i\n", prefix);

    for (int i = prefix / 8; i < 4; i++)
    {
        for (int j = prefix % 8; j < 8; j++)
        {
            binaryIP[i][j] = '0';
        }
    }

    // Convert IP from binary to decimal to be able to display network address
    int networkIP[4];

    for (int i = 0; i < 4; i++)
    {
        networkIP[i] = binary_to_num(binaryIP[i]);
    }

    printf("Network Address: %i.%i.%i.%i\n", networkIP[0], networkIP[1], networkIP[2], networkIP[3]);


    /*
     If no subnet bits present, return
     No subnet bits present if IP class is equal to it's default prefix
    */
    if (splitIP[0] > 126 && prefix == 8 || splitIP[0] > 191 && prefix == 16 || splitIP[0] > 223 && prefix == 24)
    {
        printf("Usable Host Addresses: %i\n", (int)pow(2, 32 - prefix) - 2);
        return 0;
    }

    // Calculate which octet to start counting subnet bits
    int subnetOctetStart;

    if (networkIP[0] <= 126)
    {
        subnetOctetStart = 1;
    }
    else if (networkIP[0] <= 191)
    {
        subnetOctetStart = 2;
    }
    else if (networkIP[0] <= 223)
    {
        subnetOctetStart = 3;
    }

    // Calculate amount of subnet bits '1's and host bits '0's in subnet mask

    int subnetBits = 0;
    int hostBits = 0;
    int finalOctet;

    for (int i = subnetOctetStart; i < 4; i++)
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
    }

    /*
     Calculating magic number is different for each ip class
     Magic number is the power of the last subnet bit

    */ 


    int networks;
    int magicNumber;
    int usableHosts;
    int calculateMN;

    networks = pow(2, subnetBits);


    // If class A and subnet bits is more than 2 octets
    if (subnetBits > 16 && subnetOctetStart == 1)
    {
        calculateMN = pow(2, subnetBits - 16);
    }
    // If class A and subnet bits is more than 1 octet
    else if (subnetBits > 8 && subnetOctetStart == 1)
    {
        calculateMN = pow(2, subnetBits - (subnetOctetStart * 8));
    }
    // If class B and subnet bits is more than 1 octet
    else if (subnetBits > 8 && subnetOctetStart == 2)
    {
        calculateMN = pow(2, subnetBits - ((subnetOctetStart - 1) * 8));
    }
    else 
    {
        calculateMN = networks;
    }

    magicNumber = 256 / calculateMN;

    // Calculate and display usable host addresses
    usableHosts = pow(2, hostBits) - 2;

    printf("Usable Host Addresses/Subnet: %i\n", usableHosts);

    int subnetAddress[4];

    // Copy network IP address into subnetAddress
    for (int i = 0; i < 4; i++)
    {
        subnetAddress[i] = networkIP[i];
    }

    subnetAddress[finalOctet] = 0;

    printf("\nAll %i Possible /%i Networks For %i.%i.%i.%i\n", networks, prefix, networkIP[0], networkIP[1], networkIP[2], networkIP[3]);

    int count = 0;

    /*
     If only one octet increases

     Example:
     First subnet address -> Last subnet address
     10.0.0.0 -> 10.0.0.128
    */
    if (subnetBits <= 8)
    {
        // Increase octet with last subnet bit
        display_subnets(magicNumber, finalOctet, subnetAddress);
    } 
    /*
     If two octets increase

     Example:
     First subnet address -> Last subnet address
     10.0.0.0 -> 10.0.255.128
    */
    else if (subnetBits > 8 && subnetBits < 16)
    {
        int oneCount = 0;
        subnetAddress[finalOctet-1] = oneCount;
        
        // Increase octet before octet with last subnet bit by 1
        while (oneCount < 256)
        {
            subnetAddress[finalOctet-1] = oneCount;
            count = 0;

            subnetAddress[finalOctet] = 0;
            printf("%i.%i.%i.%i\n", subnetAddress[0], subnetAddress[1], subnetAddress[2], subnetAddress[3]);

            // Increase octet with last subnet bit
            display_subnets(magicNumber, finalOctet, subnetAddress);

            oneCount += 1;
        }
    }

    /*
     If three octets increase

     Example:
     First subnet address -> Last subnet address
     10.0.0.0 -> 10.255.255.128
    */
    else if (subnetBits > 16 && subnetOctetStart == 1)
    {
        subnetAddress[finalOctet-2] = 0;
        subnetAddress[finalOctet-1] = 0;

        // Increase octet with last subnet bit -2 by 1
        while (subnetAddress[finalOctet-2] < 256)
        {
            subnetAddress[finalOctet-1] = 0;

            // Increase octet before octet with last subnet bit by 1
            while (subnetAddress[finalOctet-1] < 256)
            {
                count = 0;

                subnetAddress[finalOctet] = 0;
                printf("%i.%i.%i.%i\n", subnetAddress[0], subnetAddress[1], subnetAddress[2], subnetAddress[3]);

                // Increase octet with last subnet bit by magic number
                display_subnets(magicNumber, finalOctet, subnetAddress);

                subnetAddress[finalOctet-1] += 1;
            }

            subnetAddress[finalOctet-2] += 1;
        }
        
    }

    // Free malloced memory
    free(splitIP);

    return 0;
}