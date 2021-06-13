/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#include <autoconf.h>
#include <camkes.h>
#include <stdio.h>
#include <virtqueue.h>
#include <camkes/virtqueue.h>
#include <utils/util.h>
#include <string.h>

#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#define ICMP_MSG_SIZE 64 - sizeof(struct icmphdr)
#define IPV4_LENGTH 4

virtqueue_device_t flagger_recv_virtqueue;
virtqueue_driver_t flagger_send_virtqueue;
void handle_flagger_recv_callback(virtqueue_device_t *vq);
void handle_flagger_send_callback(virtqueue_driver_t *vq);

int send_outgoing_packet(char *outgoing_data, size_t outgoing_data_size, virtqueue_driver_t destination)
{
    void *buf = NULL;
    int err = camkes_virtqueue_buffer_alloc(&destination, &buf, outgoing_data_size);
    if (err) {
        return -1;
    }
    memcpy(buf, outgoing_data, outgoing_data_size);
    if (camkes_virtqueue_driver_send_buffer(&destination, buf, outgoing_data_size) != 0) {
        camkes_virtqueue_buffer_free(&destination, buf);
        return -1;
    }
    destination.notify();
    return 0;
}

void issue_key( virtqueue_driver_t destination )
{
    // dummy key
    char* key = "1234512345";
    send_outgoing_packet( key, 10, destination );
}

void print_ip_packet(void *ip_buf, size_t ip_length)
{
    struct iphdr *ip = ip_buf;
    struct icmphdr *icmp = ip_buf + sizeof(struct iphdr);

    unsigned char *ip_packet = (unsigned char *)ip_buf;
    printf("Packet Contents:");
    for (int i = 0; i < ip_length; i++) {
        if (i % 15 == 0) {
            printf("\n%d:\t", i);
        }
        printf("%x ", ip_packet[i]);
    }
    printf("\n");

    struct in_addr saddr = {ip->saddr};
    struct in_addr daddr = {ip->daddr};
    printf("IP Header - Version: IPv%d protocol: %d | src address: %s",
           ip->version, ip->protocol, inet_ntoa(saddr));
    printf(" | dest address: %s\n", inet_ntoa(daddr));
    printf("ICMP Header - Type: %d | id: %d | seq: %d\n",
           icmp->type, icmp->un.echo.id, icmp->un.echo.sequence);
    printf("\n");
}

void handle_recv_data(char *recv_data, size_t recv_data_size)
{
    struct ethhdr *rcv_req = (struct ethhdr *) recv_data;
    if (ntohs(rcv_req->h_proto) == ETH_P_ARP) {
    } else if (ntohs(rcv_req->h_proto) == ETH_P_IP) {
        char ip_packet[ETHERMTU];
        memcpy(ip_packet, recv_data + sizeof(struct ethhdr), recv_data_size - sizeof(struct ethhdr));
        print_ip_packet(ip_packet, recv_data_size - sizeof(struct ethhdr));
    }
}

// received from vm via flagger,
// respond with a key
void handle_flagger_recv_callback(virtqueue_device_t *vq)
{
    void *buf = NULL;
    size_t buf_size = 0;
    vq_flags_t flag;
    virtqueue_ring_object_t handle;
    if (!virtqueue_get_available_buf(vq, &handle)) {
        ZF_LOGE("Client virtqueue dequeue failed");
        return;
    }

    while (camkes_virtqueue_device_gather_buffer(vq, &handle, &buf, &buf_size, &flag) >= 0) {
        issue_key( flagger_send_virtqueue );
    }

    if (!virtqueue_add_used_buf(&flagger_recv_virtqueue, &handle, 0)) {
        ZF_LOGE("Unable to enqueue used flagger_recv buffer");
        return;
    }

    flagger_recv_virtqueue.notify();
}

void handle_flagger_send_callback(virtqueue_driver_t *vq)
{
    void *buf = NULL;
    size_t buf_size = 0, wr_len = 0;
    vq_flags_t flag;
    virtqueue_ring_object_t handle;
    if (!virtqueue_get_used_buf(vq, &handle, &wr_len)) {
        ZF_LOGE("Client virtqueue dequeue failed");
        return;
    }

    while (camkes_virtqueue_driver_gather_buffer(vq, &handle, &buf, &buf_size, &flag) >= 0) {
        /* Clean up and free the buffer we allocated */
        camkes_virtqueue_buffer_free(vq, buf);
    }
}

void msg_wait_callback(void)
{
    if (VQ_DEV_POLL(&flagger_recv_virtqueue)) {
        handle_flagger_recv_callback(&flagger_recv_virtqueue);
    }

    if (VQ_DRV_POLL(&flagger_send_virtqueue)) {
        handle_flagger_send_callback(&flagger_send_virtqueue);
    }
}

int run(void)
{
    ZF_LOGE("Starting key manager component");

    // for flagger!
    /* Initialise recv virtqueue */
    int err = camkes_virtqueue_device_init(&flagger_recv_virtqueue, 10);
    if (err) {
        ZF_LOGE("Unable to initialise flagger_recv virtqueue");
        return 1;
    }

    /* Initialise send virtqueue */
    err = camkes_virtqueue_driver_init(&flagger_send_virtqueue, 11);
    if (err) {
        ZF_LOGE("Unable to initialise flagger_send virtqueue");
        return 1;
    }

    // send a packet to ping_client "greetings"
    /*
    char* test = "Greetings!";
    size_t test_len = strlen(test);
    send_outgoing_packet( test, test_len, flagger_send_virtqueue );
    */

    return 0;
}
