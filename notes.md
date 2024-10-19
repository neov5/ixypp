### ixy notes

```
struct ixy_device {
	const char* pci_addr;
	const char* driver_name;
	uint16_t num_rx_queues;
	uint16_t num_tx_queues;

	uint32_t (*rx_batch) (struct ixy_device* dev, uint16_t queue_id, struct pkt_buf* bufs[], uint32_t num_bufs);
	uint32_t (*tx_batch) (struct ixy_device* dev, uint16_t queue_id, struct pkt_buf* bufs[], uint32_t num_bufs);
	void (*read_stats) (struct ixy_device* dev, struct device_stats* stats);
	void (*set_promisc) (struct ixy_device* dev, bool enabled); // set promiscuous
	uint32_t (*get_link_speed) (const struct ixy_device* dev);
	struct mac_address (*get_mac_addr) (const struct ixy_device* dev);

	void (*set_mac_addr) (struct ixy_device* dev, struct mac_address mac);
	bool vfio;
	int vfio_fd; // device fd
	struct interrupts interrupts;
};

struct virtio_device {
	struct ixy_device ixy;
	int fd;
	void* rx_queue;
	void* tx_queue;
	void* ctrl_queue;
	uint64_t rx_pkts;
	uint64_t tx_pkts;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
};
```

basically inheritance-based polymorphism but in C

init -> open NIC and check vendor id, then delegate to virtio/ixgbe init
allocates and reutrns an ixy_device with function ptrs set appropriately 

I'll do the ixgbe one, because the virtio one is a bit hard to follow

rx_batch -> bascially reads a bunch of packeet

this seems super simple. Basically init the virtio driver, read/write to it's 
virtqueues, expose an API and we're good. Seems the same as making a driver. 
Where are the userspace specific things happening?
