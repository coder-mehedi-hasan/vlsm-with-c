# VLSM with C

A simple **VLSM (Variable Length Subnet Mask) calculator** implemented in **C**.  
This program helps calculate subnets efficiently by dividing an IP address into multiple subnets based on required hosts. Ideal for networking students and engineers.

## Features

- Calculate **network address**, **broadcast address**, and **subnet mask** for each subnet.  
- Supports IP input with or without **CIDR notation**.  
- Allocates subnets using **VLSM** for efficient IP usage.  
- Lightweight and easy-to-use **C program**.

## How to Use

1. Enter a valid IP address.  
2. Specify the number of subnets required.  
3. Enter the number of hosts needed for each subnet.  
4. The program calculates and displays:

   - Network addresses  
   - Broadcast addresses  
   - Subnet masks (CIDR)

## Example

```
IP address: 192.168.1.0
Networks needed: 3
Hosts for Network 1: 50
Hosts for Network 2: 20
Hosts for Network 3: 10

Results:
Network 1: 192.168.1.0/26  Broadcast: 192.168.1.63
Network 2: 192.168.1.64/27 Broadcast: 192.168.1.95
Network 3: 192.168.1.96/28 Broadcast: 192.168.1.111
```
## Keywords for SEO
VLSM, VLSM calculator, C program, subnetting calculator, IP subnet calculator, network calculator, CIDR, variable length subnet mask, networking C program.
