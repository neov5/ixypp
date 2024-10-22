#include "virtio.hpp"

namespace ixypp {

void virtio_driver::init(std::string pci_device) {
    // TODO init virtio device
}

std::vector<packet> virtio_driver::rx_batch() {
    // TODO receive a batch of packets from virtio device
}

void virtio_driver::tx_batch(std::vector<packet> packets) {
    // TODO transmit a batch of packets to virtio device
}

virtio_driver::~virtio_driver() {

}

};
