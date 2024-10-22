#pragma once

#include <cinttypes>
#include <memory>
#include <vector>

namespace ixypp {

struct packet {
    size_t size;
    std::unique_ptr<uint8_t> data;
};

class driver {
public:
    virtual void init(std::string pci_device) = 0;
    virtual std::vector<packet> rx_batch() = 0;
    virtual void tx_batch(std::vector<packet>) = 0;
    virtual ~driver() = 0;
};

};
