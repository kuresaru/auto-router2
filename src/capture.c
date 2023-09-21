#include <stdint.h>
#include <pcap.h>
#include "main.h"

const static char filter_exp[] = "tcp[tcpflags]&(tcp-syn)==tcp-syn or tcp[tcpflags]&(tcp-syn|tcp-ack)==(tcp-syn|tcp-ack)";

static void process_packet(uint8_t *args, const struct pcap_pkthdr *hdr, const uint8_t *pkt)
{
    if (hdr->len >= (14 + 20 + 40)) // eth+ipv4+tcp
    {
        if (ntohs(*(uint16_t *)(pkt + 12)) == 0x0800) // eth.type: IPv4
        {
            const uint8_t *ip_hdr = pkt + 14;
            if (((ip_hdr[0] & 0xF0) == 0x40) && (ip_hdr[9] == 6)) // ip.version: 4 and ip.protocol: TCP
            {
                const uint8_t ip_hdr_len = (ip_hdr[0] & 0x0F) * 4;
                const uint8_t *tcp_hdr = ip_hdr +ip_hdr_len;
                const uint8_t tcp_flags_syn_ack = tcp_hdr[13] & 0x12;
                if (tcp_flags_syn_ack == 0x02) // SYN
                {
                    uint32_t ip_dst = ntohl(*((uint32_t *)(ip_hdr + 16)));
                    mark_syn(&ip_dst);
                }
                else if (tcp_flags_syn_ack == 0x12) // SYN,ACK
                {
                    uint32_t ip_src = ntohl(*((uint32_t *)(ip_hdr + 12)));
                    mark_ack(&ip_src);
                }
            }
        }
    }
}

int capture_start()
{
    char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t *ph = pcap_open_live(capture_interface, BUFSIZ, 1, 1000, error_buffer);
    if (!ph)
    {
        fprintf(stderr, "failed to open eth %s: %s\n", capture_interface, error_buffer);
        return 1;
    }
    struct bpf_program filter;
    bpf_u_int32 mask, ip;
    pcap_lookupnet(capture_interface, &ip, &mask, error_buffer);
    pcap_compile(ph, &filter, filter_exp, 0, ip);
    pcap_setfilter(ph, &filter);
    printf("start sniffer network traffic on %s\n", capture_interface);
    pcap_loop(ph, -1, process_packet, NULL);
    return 0;
}