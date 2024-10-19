#pragma once

#include "../driver.hpp"
#include "../endianint.hpp"
#include <stdint.h> 

using le64 = endianint<endian::little, uint64_t>;
using le32 = endianint<endian::little, uint32_t>;
using le16 = endianint<endian::little, uint16_t>;

// Taken from the oasis virtqueue header

/* This marks a buffer as continuing via the next field. */ 
constexpr uint16_t VIRTQ_DESC_F_NEXT = 1;
/* This marks a buffer as write-only (otherwise read-only). */ 
constexpr uint16_t VIRTQ_DESC_F_WRITE = 2; 
/* This means the buffer contains a list of buffer descriptors. */ 
constexpr uint16_t VIRTQ_DESC_F_INDIRECT = 4; 
 
/* The device uses this in used->flags to advise the driver: don’t kick me 
 * when you add a buffer.  It’s unreliable, so it’s simply an 
 * optimization. */ 
constexpr uint16_t VIRTQ_USED_F_NO_NOTIFY = 1; 
/* The driver uses this in avail->flags to advise the device: don’t 
 * interrupt me when you consume a buffer.  It’s unreliable, so it’s 
 * simply an optimization.  */ 
constexpr uint16_t VIRTQ_AVAIL_F_NO_INTERRUPT = 1; 
 
/* Support for indirect descriptors */ 
constexpr uint16_t VIRTIO_F_INDIRECT_DESC = 28; 
 
/* Support for avail_event and used_event fields */ 
constexpr uint16_t VIRTIO_F_EVENT_IDX = 29; 
 
/* Arbitrary descriptor layouts. */ 
constexpr uint16_t VIRTIO_F_ANY_LAYOUT = 27; 
 
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

namespace ixypp {

class driver_virtio_t : public driver_t {

public:
    void init();

};

};
