#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <libnfnetlink/libnfnetlink.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/ip.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <stdint.h>
#include <arpa/nameser.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <unistd.h>
#include <arpa/nameser.h>
#include <sys/types.h>
#include <resolv.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <stddef.h>
#define DNS_QUEUE 9

int first_connect = 1;
int clientSocket;
char buffer[1024];
struct sockaddr_in serverAddr;
socklen_t addr_size;

int connect_to_server(){

	/*---- Create the socket. The three arguments are: ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	
	/*---- Configure settings of the server address struct ----*/
	/* Address family = Internet */
	serverAddr.sin_family = AF_INET;
	/* Set port number, using htons function to use proper byte order */
	serverAddr.sin_port = htons(65432);
	/* Set IP address to localhost */
	serverAddr.sin_addr.s_addr = inet_addr("192.168.31.128");
	/* Set all bits of the padding field to 0 */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
	
	/*---- Connect the socket to the server using the address struct ----*/
	addr_size = sizeof serverAddr;
	int val = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
	return val;
}

int dns_parse(const unsigned char *msg, size_t len)
{
    ns_msg handle;
    ns_rr rr;
    int i,rdlen;
    char astr[INET6_ADDRSTRLEN]={0};
    bool ns_t_a_found = false;
    bool ns_t_aaaa_found = false;

    if (ns_initparse(msg, len, &handle) != 0) {
        //printf("Unable to parse dns payload\n");
        return 0;
    }

    if(ns_msg_getflag(handle, ns_f_qr) == 0)
    {   
        for (i = 0; i < ns_msg_count(handle, ns_s_qd); i++)
        {
        if (ns_parserr(&handle, ns_s_qd, i, &rr) != 0) {
            printf("Unable to parse resource record: %s\n", strerror(errno));
            return 0;
        }

        //printf("Name:\t%s\n", ns_rr_name(rr));
        
        }
        if (first_connect){
            connect_to_server();
            first_connect = 0;
        }
        char *host;
        host = ns_rr_name(rr);
        //int host_len = ns_rr_rdlen(rr);
        //printf("host : %s\n",host);
        send(clientSocket, host, strlen(host), 0);
        /*---- Read the message from the server into the buffer ----*/
        int bal = recv(clientSocket, buffer, 1024, 0);
        //printf("Present in database: %s\n",buffer);
        char string1[] = "yes";
        if(!strcmp(string1,buffer)){
            return 1;
        }
        else{
            return 0;
        }

    }

    return 0;

}

static int dns_packetHandler(struct nfq_q_handle * myQueue, struct nfgenmsg *msg,struct nfq_data *pkt, void *cbData)
{
    uint32_t pkt_queue_id = -1;
    struct nfqnl_msg_packet_hdr *header=NULL;
    unsigned char * packet = NULL;
    int len = 0;
    int i,j;
    if ((header = nfq_get_msg_packet_hdr(pkt))) {
    	pkt_queue_id = ntohl(header->packet_id);
	//printf("packet ID in queue is %d\n", pkt_queue_id);
    }
    
    if((len = nfq_get_payload(pkt, &packet)) == -1) {
        printf("nfq_get_payload failed");
    }
    

    /* Pointers to start point of various headers */
    const u_char *ip_header;
    const u_char *udp_header;
    const u_char *payload;

    /* Header lengths in bytes */
    int ip_header_length;
    int udp_header_length = 8;
    int payload_length;

    /* Find start of IP header */
    ip_header = packet ;
    /* The second-half of the first byte in ip_header
       contains the IP header length (IHL). */
    ip_header_length = ((*ip_header) & 0x0F);
    /* The IHL is number of 32-bit segments. Multiply
       by four to get a byte count for pointer arithmetic */
    ip_header_length = ip_header_length * 4;
   // printf("IP header length (IHL) in bytes: %d\n", ip_header_length);

    u_char protocol = *(ip_header + 9);

    /* Add the ethernet and ip header length to the start of the packet
       to find the beginning of the TCP header */
    udp_header = packet + ip_header_length;
    //printf("UDP source port: %d\n", htons(*(uint16_t *)udp_header));

    //printf("UDP dest port: %d\n", htons(*(uint16_t *)(udp_header+2)));
    uint16_t udp_length = htons(*(uint16_t *)(udp_header+4));

    int total_headers_size = ip_header_length+udp_header_length;
    //printf("Size of all headers combined: %d bytes\n", total_headers_size);
    payload_length = len - ( ip_header_length + udp_header_length);
    //printf("Payload size: %d bytes\n", payload_length);
    payload = packet + total_headers_size;
   // printf("Memory address where payload begins: %p %c\n\n", payload,*payload);

    if(dns_parse(payload, payload_length)){
        nfq_set_verdict(myQueue, pkt_queue_id, NF_DROP, 0, NULL);
        printf("verdict set as drop for packet id %d\n", pkt_queue_id);
    }
    else{
        nfq_set_verdict(myQueue, pkt_queue_id, NF_ACCEPT, 0, NULL);
        //printf("verdict set as accept for packet id %d\n", pkt_queue_id);
    }
    return 0;
}

int main()
{
    struct nfq_handle *nfqHandle = NULL;
    struct nfq_q_handle *myQueue = NULL;
    struct nfnl_handle *netlinkHandle = NULL;
    int status = 0;
    int fd = 0, res = 0;
    char buf[4096] = {0};

    // Get a queue connection handle
    if (!(nfqHandle = nfq_open())) {
        printf("Error in nfq_open()\n");
        exit(1);
    }

    // Unbind the handler from processing any IP packets
    if ((status = nfq_unbind_pf(nfqHandle, AF_INET)) < 0) {
        printf("Error in nfq_unbind_pf(): %d\n", status);
 //       exit(1);
   }

    // Bind this handler to process IP packets
    if ((status = nfq_bind_pf(nfqHandle, AF_INET)) < 0) {
        printf("Error in nfq_bind_pf(): %d\n", status);
 //       exit(1);
    }
    // Install a callback on each of the iptables NFQUEUE queues
    if (!(myQueue = nfq_create_queue(nfqHandle,  DNS_QUEUE, &dns_packetHandler, NULL))) {

        printf("Error in nfq_create_queue(): %p\n", myQueue);
        exit(1);
    } else {
        printf("Registered packet handler for queue %d\n", DNS_QUEUE);

        // Turn on packet copy mode
        if ((status = nfq_set_mode(myQueue, NFQNL_COPY_PACKET, 0xffff)) < 0) {

            printf("Error in nfq_set_mode(): %d\n", status);
            exit(1);
        }
    }
    netlinkHandle = nfq_nfnlh(nfqHandle);
    fd = nfnl_fd(netlinkHandle);
    while ((res = recv(fd, buf, sizeof(buf), 0)) && res >= 0)
	{
        //printf("Call nfq_handle_packet\n");
        nfq_handle_packet(nfqHandle, buf, res);
    }
    
}
