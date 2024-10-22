#pragma once

#include "../driver.hpp"
#include "../endianint.hpp"
#include <stdint.h> 

using le64 = endianint<endian::little, uint64_t>;
using le32 = endianint<endian::little, uint32_t>;
using le16 = endianint<endian::little, uint16_t>;
using u8 = uint8_t;
using u16 = uint16_t;

namespace virtio {

// Taken from the oasis virtqueue header

/* This marks a buffer as continuing via the next field. */ 
constexpr uint16_t VIRTQ_DESC_F_NEXT = 1;
/* This marks a buffer as write-only (otherwise read-only). */ 
constexpr uint16_t VIRTQ_DESC_F_WRITE = 2; 
/* This means the buffer contains a list of buffer descriptors. */ 
constexpr uint16_t VIRTQ_DESC_F_INDIRECT = 4; 

/* To mark a descriptor as available and used, both the driver and the device 
 * use the following two flags: */
constexpr uint16_t VIRTQ_DESC_F_AVAIL = (1 << 7);
constexpr uint16_t VIRTQ_DESC_F_USED = (1 << 7);

/* Enable events */ 
constexpr uint16_t RING_EVENT_FLAGS_ENABLE = 0x0;
/* Disable events */ 
constexpr uint16_t RING_EVENT_FLAGS_DISABLE = 0x1;
/* 
 * Enable events for a specific descriptor 
 * (as specified by Descriptor Ring Change Event Offset/Wrap Counter). 
 * Only valid if VIRTIO_F_EVENT_IDX has been negotiated. 
 */ 
constexpr uint16_t RING_EVENT_FLAGS_DESC = 0x2;
/* The value 0x3 is reserved */
 
/* The device uses this in used->flags to advise the driver: don’t kick me 
 * when you add a buffer.  It’s unreliable, so it’s simply an 
 * optimization. */ 
constexpr uint16_t VIRTQ_USED_F_NO_NOTIFY = 1; 
/* The driver uses this in avail->flags to advise the device: don’t 
 * interrupt me when you consume a buffer.  It’s unreliable, so it’s 
 * simply an optimization.  */ 
constexpr uint16_t VIRTQ_AVAIL_F_NO_INTERRUPT = 1; 
 
/* Support for indirect descriptors */ 
constexpr uint16_t F_INDIRECT_DESC = 28; 
 
/* Support for avail_event and used_event fields */ 
constexpr uint16_t F_EVENT_IDX = 29; 
 
/* Arbitrary descriptor layouts. */ 
constexpr uint16_t F_ANY_LAYOUT = 27; 

/* Indicates that the guest OS has found the device and recognized it as a valid virtio device. */
constexpr uint16_t STATUS_ACKNOWLEDGE = 1;

/* Indicates that the guest OS knows how to drive the device. Note: There could be a significant (or infinite) delay before setting this bit. For example, under Linux, drivers can be loadable modules. */
constexpr uint16_t STATUS_DRIVER = 2;

/* Indicates that something went wrong in the guest, and it has given up on the device. This could be an internal error, or the driver didn’t like the device for some reason, or even a fatal error during device operation. */
constexpr uint16_t STATUS_FAILED = 128;

/* Indicates that the driver has acknowledged all the features it understands, and feature negotiation is complete. */
constexpr uint16_t STATUS_FEATURES_OK = 8;

/* Indicates that the driver is set up and ready to drive the device. */
constexpr uint16_t STATUS_DRIVER_OK = 4;

/* Indicates that the device has experienced an error from which it can’t recover.*/
constexpr uint16_t STATUS_DEVICE_NEEDS_RESET = 64;

/* Virtqueue descriptors: 16 bytes. 
 * These can chain together via "next". */ 
struct virtq_desc { 
    /* Address (guest-physical). */ 
    le64 addr; 
    /* Length. */ 
    le32 len; 
    /* The flags as indicated above. */ 
    le16 flags; 
    /* We chain unused descriptors via this, too */ 
    le16 next; 
}; 

struct pvirtq_desc { 
    /* Buffer Address. */ 
    le64 addr; 
    /* Buffer Length. */ 
    le32 len; 
    /* Buffer ID. */ 
    le16 id; 
    /* The flags depending on descriptor type. */ 
    le16 flags; 
};

// TODO model this 
// struct pvirtq_event_suppress { 
//     le16 { 
//          desc_event_off : 15; /* Descriptor Ring Change Event Offset */ 
//          desc_event_wrap : 1; /* Descriptor Ring Change Event Wrap Counter */ 
//     } desc; /* If desc_event_flags set to RING_EVENT_FLAGS_DESC */ 
//     le16 { 
//          desc_event_flags : 2, /* Descriptor Ring Change Event Flags */ 
//          reserved : 14; /* Reserved, set to 0 */ 
//     } flags; 
// };
 
struct virtq_avail { 
    le16 flags; 
    le16 idx; 
    le16 ring[]; 
    /* Only if VIRTIO_F_EVENT_IDX: le16 used_event; */ 
}; 
 
/* le32 is used here for ids for padding reasons. */ 
struct virtq_used_elem { 
    /* Index of start of used descriptor chain. */ 
    le32 id; 
    /* Total length of the descriptor chain which was written to. */ 
    le32 len; 
}; 
 
struct virtq_used { 
    le16 flags; 
    le16 idx; 
    struct virtq_used_elem ring[]; 
    /* Only if VIRTIO_F_EVENT_IDX: le16 avail_event; */ 
}; 
 
struct virtq { 
    unsigned int num; 
 
    struct virtq_desc *desc; 
    struct virtq_avail *avail; 
    struct virtq_used *used; 
}; 
 
static inline int virtq_need_event(uint16_t event_idx, uint16_t new_idx, uint16_t old_idx) 
{ 
     return (uint16_t)(new_idx - event_idx - 1) < (uint16_t)(new_idx - old_idx); 
} 
 
/* Get location of event indices (only with VIRTIO_F_EVENT_IDX) */ 
static inline le16 *virtq_used_event(struct virtq *vq) 
{ 
    /* For backwards compat, used event index is at *end* of avail ring. */ 
    return &vq->avail->ring[vq->num]; 
} 
 
static inline le16 *virtq_avail_event(struct virtq *vq) 
{ 
    /* For backwards compat, avail event index is at *end* of used ring. */ 
    return (le16 *)&vq->used->ring[vq->num]; 
} 

/* TODO
#define ALIGN(x) (((x) + qalign) & "qalign) 
static inline unsigned virtq_size(unsigned int qsz) 
{ 
     return ALIGN(sizeof(struct virtq_desc)*qsz + sizeof(uint16_t)*(3 + qsz)) 
      + ALIGN(sizeof(u16)*3 + sizeof(struct virtq_used_elem)*qsz); 
}
*/

/* Common configuration */ 
constexpr u8 VIRTIO_PCI_CAP_COMMON_CFG = 1;
/* Notifications */ 
constexpr u8 VIRTIO_PCI_CAP_NOTIFY_CFG = 2;
/* ISR Status */ 
constexpr u8 VIRTIO_PCI_CAP_ISR_CFG = 3;
/* Device specific configuration */ 
constexpr u8 VIRTIO_PCI_CAP_DEVICE_CFG = 4;
/* PCI configuration access */ 
constexpr u8 VIRTIO_PCI_CAP_PCI_CFG = 5;
/* Shared memory region */ 
constexpr u8 VIRTIO_PCI_CAP_SHARED_MEMORY_CFG = 8;
/* Vendor-specific data */ 
constexpr u8 VIRTIO_PCI_CAP_VENDOR_CFG = 9;

struct pci_cap { 
    u8 cap_vndr;    /* Generic PCI field: PCI_CAP_ID_VNDR */ 
    u8 cap_next;    /* Generic PCI field: next ptr. */ 
    u8 cap_len;     /* Generic PCI field: capability length */ 
    u8 cfg_type;    /* Identifies the structure. */ 
    u8 bar;     /* Where to find it. */ 
    u8 id;      /* Multiple capabilities of the same type */ 
    u8 padding[2];  /* Pad to full dword. */ 
    le32 offset;    /* Offset within bar. */ 
    le32 length;    /* Length of the structure, in bytes. */ 
};

struct virtio_pci_common_cfg { 
    /* About the whole device. */ 
    le32 device_feature_select;     /* read-write */ 
    le32 device_feature;        /* read-only for driver */ 
    le32 driver_feature_select;     /* read-write */ 
    le32 driver_feature;        /* read-write */ 
    le16 config_msix_vector;    /* read-write */ 
    le16 num_queues;        /* read-only for driver */ 
    u8 device_status;           /* read-write */ 
    u8 config_generation;       /* read-only for driver */ 
 
    /* About a specific virtqueue. */ 
    le16 queue_select;          /* read-write */ 
    le16 queue_size;        /* read-write */ 
    le16 queue_msix_vector;     /* read-write */ 
    le16 queue_enable;          /* read-write */ 
    le16 queue_notify_off;      /* read-only for driver */ 
    le64 queue_desc;        /* read-write */ 
    le64 queue_driver;          /* read-write */ 
    le64 queue_device;          /* read-write */ 
    le16 queue_notif_config_data;   /* read-only for driver */ 
    le16 queue_reset;           /* read-write */ 
 
    /* About the administration virtqueue. */ 
    le16 admin_queue_index;     /* read-only for driver */ 
    le16 admin_queue_num;     /* read-only for driver */ 
};

namespace net {

/* Device handles packets with partial checksum offload. */
constexpr u8 VIRTIO_NET_F_CSUM = 0;

/* Driver handles packets with partial checksum. */
constexpr u8 VIRTIO_NET_F_GUEST_CSUM = 1;

/* Control channel offloads reconfiguration support. */
constexpr u8 VIRTIO_NET_F_CTRL_GUEST_OFFLOADS = 2;

/* Device maximum MTU reporting is supported. If offered by the device, device advises driver about the value of its maximum MTU. If negotiated, the driver uses mtu as the maximum MTU value. */
constexpr u8 VIRTIO_NET_F_MTU= 3;

/* Device has given MAC address. */
constexpr u8 VIRTIO_NET_F_MAC = 5;

/* Driver can receive TSOv4. */
constexpr u8 VIRTIO_NET_F_GUEST_TSO4 = 7;

/* Driver can receive TSOv6. */
constexpr u8 VIRTIO_NET_F_GUEST_TSO6 = 8;

/* Driver can receive TSO with ECN. */
constexpr u8 VIRTIO_NET_F_GUEST_ECN = 9;

/* Driver can receive UFO. */
constexpr u8 VIRTIO_NET_F_GUEST_UFO = 10;

/* Device can receive TSOv4. */
constexpr u8 VIRTIO_NET_F_HOST_TSO4 = 11;

/* Device can receive TSOv6. */
constexpr u8 VIRTIO_NET_F_HOST_TSO6 = 12;

/* Device can receive TSO with ECN. */
constexpr u8 VIRTIO_NET_F_HOST_ECN = 13;

/* Device can receive UFO. */
constexpr u8 VIRTIO_NET_F_HOST_UFO = 14;

/* Driver can merge receive buffers. */
constexpr u8 VIRTIO_NET_F_MRG_RXBUF = 15;

/* Configuration status field is available. */
constexpr u8 VIRTIO_NET_F_STATUS = 16;

/* Control channel is available. */
constexpr u8 VIRTIO_NET_F_CTRL_VQ = 17;

/* Control channel RX mode support. */
constexpr u8 VIRTIO_NET_F_CTRL_RX = 18;

/* Control channel VLAN filtering. */
constexpr u8 VIRTIO_NET_F_CTRL_VLAN = 19;

/* Control channel RX extra mode support. */
constexpr u8 VIRTIO_NET_F_CTRL_RX_EXTRA = 20;

/* Driver can send gratuitous packets. */
constexpr u8 VIRTIO_NET_F_GUEST_ANNOUNCE= 21;

/* Device supports multiqueue with automatic receive steering. */
constexpr u8 VIRTIO_NET_F_MQ= 22;

/* Set MAC address through control channel. */
constexpr u8 VIRTIO_NET_F_CTRL_MAC_ADDR= 23;

/* Device supports inner header hash for encapsulated packets. */
constexpr u8 VIRTIO_NET_F_HASH_TUNNEL= 51;

/* Device supports virtqueue notification coalescing. */
constexpr u8 VIRTIO_NET_F_VQ_NOTF_COAL= 52;

/* Device supports notifications coalescing. */
constexpr u8 VIRTIO_NET_F_NOTF_COAL= 53;

/* Driver can receive USOv4 packets. */
constexpr u8 VIRTIO_NET_F_GUEST_USO4 = 54;

/* Driver can receive USOv6 packets. */
constexpr u8 VIRTIO_NET_F_GUEST_USO6 = 55;

/* Device can receive USO packets. Unlike UFO (fragmenting the packet) the USO 
 * splits large UDP packet to several segments when each of these smaller 
 * packets has UDP header. */
constexpr u8 VIRTIO_NET_F_HOST_USO = 56;

/* Device can report per-packet hash value and a type of calculated hash. */
constexpr u8 VIRTIO_NET_F_HASH_REPORT= 57;

/* Driver can provide the exact hdr_len value. Device benefits from knowing 
 * the exact header length. */
constexpr u8 VIRTIO_NET_F_GUEST_HDRLEN= 59;

/* Device supports RSS (receive-side scaling) with Toeplitz hash calculation 
 * and configurable hash parameters for receive steering. */
constexpr u8 VIRTIO_NET_F_RSS= 60;

/* Device can process duplicated ACKs and report number of coalesced segments 
 * and duplicated ACKs. */
constexpr u8 VIRTIO_NET_F_RSC_EXT= 61;

/* Device may act as a standby for a primary device with the same MAC address. */
constexpr u8 VIRTIO_NET_F_STANDBY= 62;

/* Device reports speed and duplex.*/
constexpr u8 VIRTIO_NET_F_SPEED_DUPLEX= 63;

constexpr u8 S_LINK_UP = 1;
constexpr u8 S_ANNOUNCE = 2;

struct config { 
    u8 mac[6]; 
    le16 status; 
    le16 max_virtqueue_pairs; 
    le16 mtu; 
    le32 speed; 
    u8 duplex; 
    u8 rss_max_key_size; 
    le16 rss_max_indirection_table_length; 
    le32 supported_hash_types; 
    le32 supported_tunnel_types; 
};

constexpr u8 HDR_F_NEEDS_CSUM = 1;
constexpr u8 HDR_F_DATA_VALID = 2;
constexpr u8 HDR_F_RSC_INFO   = 4;

constexpr u8 HDR_GSO_NONE   = 0;
constexpr u8 HDR_GSO_TCPV4  = 1;
constexpr u8 HDR_GSO_UDP    = 3;
constexpr u8 HDR_GSO_TCPV6  = 4;
constexpr u8 HDR_GSO_UDP_L4 = 5;
constexpr u8 HDR_GSO_ECN    = 0x80;

constexpr u16 HASH_TYPE_IPv4   = (1 << 0);
constexpr u16 HASH_TYPE_TCPv4  = (1 << 1);
constexpr u16 HASH_TYPE_UDPv4  = (1 << 2);
constexpr u16 HASH_TYPE_IPv6   = (1 << 3);
constexpr u16 HASH_TYPE_TCPv6  = (1 << 4);
constexpr u16 HASH_TYPE_UDPv6  = (1 << 5);
constexpr u16 HASH_TYPE_IP_EX  = (1 << 6);
constexpr u16 HASH_TYPE_TCP_EX = (1 << 7);
constexpr u16 HASH_TYPE_UDP_EX = (1 << 8);

struct virtio_net_hdr { 
    u8 flags; 
    u8 gso_type; 
    le16 hdr_len; 
    le16 gso_size; 
    le16 csum_start; 
    le16 csum_offset; 
    le16 num_buffers; 
    le32 hash_value;        /* (Only if VIRTIO_NET_F_HASH_REPORT negotiated) */ 
    le16 hash_report;       /* (Only if VIRTIO_NET_F_HASH_REPORT negotiated) */ 
    le16 padding_reserved;  /* (Only if VIRTIO_NET_F_HASH_REPORT negotiated) */ 
};

};

};

namespace ixypp {

class virtio_driver : public driver {

public:
    void init(std::string pci_device) override;
    std::vector<packet> rx_batch() override;
    void tx_batch(std::vector<packet> packets) override;
    ~virtio_driver() override;
};

};
