// this is a simple tool that ICMP pings an ip, maybe in a loop
// its just to slow down the wifi traffic being given by my belkin wifiaccess point
// to the 1/2 megabit speed needed for the DS to work with it properly
// 
// kernow 9/12/2005

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <string.h>
#include <unistd.h>

// in include/checksum.h
#include <checksum.h>

// the DS IP address
char dsIPAddress[20];

void usage() 
{
  printf("Belkin wifi + Nintendo DS hack, kernow 2005");
  printf("\nUsage: ds-belkin [Static DS IP address]\n");
  printf("Note: The default DNS name is 'NintendoDS'\n");
  
}

void checkRootUser() 
{
  if(getuid() != 0)
    {
      printf("This program must be executed as UID 0.\n\n");
      usage();
      exit (1);
    }
} 

void doICMP()
{
  // setup the ICMP packet and send it
  // code adapted from pinger.c example
  
  struct iphdr* ip;
  struct iphdr* ip_reply;
  struct icmphdr* icmp;
  struct sockaddr_in connection;
  char* packet;
  char* buffer;
  int sockfd;
  int optval;
  int addrlen;
  int siz;
  
  printf("setting up a connection to the Nintendo DS.\n");
  
  // allocate some ... stuff, yeah
  packet = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));
  buffer = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));
  ip = (struct iphdr*) packet;
  icmp = (struct icmphdr*) (packet + sizeof(struct iphdr));
  
  // set the ICMP packet structure up
  ip->ihl = 5;
  ip->version = 4;
  ip->tos = 0;
  ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
  ip->id = htons(0);
  ip->frag_off = 0;
  ip->ttl = 64;
  ip->protocol = IPPROTO_ICMP;
  ip->daddr = inet_addr(dsIPAddress);
  ip->check = in_cksum((unsigned short *)ip, sizeof(struct iphdr));
  
  // connect a socket
  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {    
      perror("socket");
      exit(EXIT_FAILURE);
    }
  
  // this is needed so that the kernel doesnt add an ip header itself
  setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));
  
  // generate more packet entries and a checksum
  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.id = random();
  icmp->un.echo.sequence = 0;
  icmp-> checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));
  connection.sin_family = AF_INET;
  connection.sin_addr.s_addr = inet_addr(dsIPAddress);
  
  // send the packet
  printf("Sending ICMP packet to Nintendo DS.\n");
  sendto(sockfd, packet, ip->tot_len, 0, (struct sockaddr *)&connection, sizeof(struct sockaddr));
  //printf("Sent %d byte packet to %s\n", ip->tot_len, dsIPAddress);
  

  // now we have to recieve a packet from the DS
  
  printf("Recieving ICMP packet from Nintendo DS.\n");
  addrlen = sizeof(connection);

  // what about this 28 bytes thing
  if (( siz = recvfrom(sockfd, buffer, sizeof(struct iphdr) + sizeof(struct icmphdr), 0, (struct sockaddr *)&connection, &addrlen)) == 28)
    { 
      printf("what the fuck?");
      perror("recv");
    }
  
  else
    { 
      printf("Received %d byte reply from %s\n", siz , dsIPAddress);
      ip_reply = (struct iphdr*) buffer;
      printf("ID: %d\n", ntohs(ip_reply->id));
      printf("TTL: %d\n", ip_reply->ttl);
    }
  
  // free up the memory used
  free(packet);
  free(buffer);
  
  // close the socket
  close(sockfd);
}






int main(int argc, char *argv[]) 
{
  
  char *tempIP;
  
  // check the user has the correct priveleges
//  checkRootUser();
  
  // parse the ds ip address from the command line arguments
  if(!(*(argv + 1)))
    {
      usage();
      return 1;
    }
  else strncpy(dsIPAddress,*(argv + 1), 15);
  

  // if you give this an ip with commas in, or a bad name, it will segfault
  // I am working on how to get around that.
  // anyway, resolve the IP regardless, even if it isnt a name
  struct hostent* h;
  h = gethostbyname(dsIPAddress);
  tempIP = inet_ntoa(*(struct in_addr *)h->h_addr);
 
  strncpy(dsIPAddress,tempIP,15);
  // an IP address is 15 chars long - isnt it?
   
  // show some information
  printf("Belkin wifi, Nintendo DS hack, kernow 2005\n");
  printf("Nintendo DS static IP Address is: %s\n", dsIPAddress);
  
  // do some work!
  doICMP();

  
 return 0;
}
