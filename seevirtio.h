struct ewma_pkt_len {
	long unsigned int internal;
};

struct virtnet_sq_stats {
	struct u64_stats_sync syncp;
	u64 packets;
	u64 bytes;
	u64 xdp_tx;
	u64 xdp_tx_drops;
	u64 kicks;
	u64 tx_timeouts;
};

struct virtnet_rq_stats {
	struct u64_stats_sync syncp;
	u64 packets;
	u64 bytes;
	u64 drops;
	u64 xdp_packets;
	u64 xdp_tx;
	u64 xdp_redirects;
	u64 xdp_drops;
	u64 kicks;
};

struct send_queue {
	struct virtqueue *vq;
	struct scatterlist sg[19];
	char name[40];
	struct virtnet_sq_stats stats;
	struct napi_struct napi;
	bool reset;
};

struct receive_queue {
	struct virtqueue *vq;
	struct napi_struct napi;
	struct bpf_prog *xdp_prog;
	struct virtnet_rq_stats stats;
	struct page *pages;
	struct ewma_pkt_len mrg_avg_pkt_len;
	struct page_frag alloc_frag;
	struct scatterlist sg[19];
	unsigned int min_buf_len;
	char name[40];
	long : 64;
	long : 64;
	long : 64;
	long : 64;
	long : 64;
	long : 64;
	struct xdp_rxq_info xdp_rxq;
};

struct virtnet_info {
	struct virtio_device *vdev;
	struct virtqueue *cvq;
	struct net_device *dev;
	struct send_queue *sq;
	struct receive_queue *rq;
	unsigned int status;
	u16 max_queue_pairs;
	u16 curr_queue_pairs;
	u16 xdp_queue_pairs;
	bool xdp_enabled;
	bool big_packets;
	unsigned int big_packets_num_skbfrags;
	bool mergeable_rx_bufs;
	bool has_rss;
	bool has_rss_hash_report;
	u8 rss_key_size;
	u16 rss_indir_table_size;
	u32 rss_hash_types_supported;
	u32 rss_hash_types_saved;
	bool has_cvq;
	bool any_header_sg;
	u8 hdr_len;
	struct delayed_work refill;
	bool refill_enabled;
	spinlock_t refill_lock;
	struct work_struct config_work;
	bool affinity_hint_set;
	struct hlist_node node;
	struct hlist_node node_dead;
	struct control_buf *ctrl;
	u8 duplex;
	u32 speed;
	u32 tx_usecs;
	u32 rx_usecs;
	u32 tx_max_packets;
	u32 rx_max_packets;
	long unsigned int guest_offloads;
	long unsigned int guest_offloads_capable;
	struct failover *failover;
};

struct vring_virtqueue_split {
	struct vring vring;
	u16 avail_flags_shadow;
	u16 avail_idx_shadow;
	struct vring_desc_state_split *desc_state;
	struct vring_desc_extra *desc_extra;
	dma_addr_t queue_dma_addr;
	size_t queue_size_in_bytes;
	u32 vring_align;
	bool may_reduce_num;
};

struct vring_virtqueue_packed {
	struct {
		unsigned int num;
		struct vring_packed_desc *desc;
		struct vring_packed_desc_event *driver;
		struct vring_packed_desc_event *device;
	} vring;
	bool avail_wrap_counter;
	u16 avail_used_flags;
	u16 next_avail_idx;
	u16 event_flags_shadow;
	struct vring_desc_state_packed *desc_state;
	struct vring_desc_extra *desc_extra;
	dma_addr_t ring_dma_addr;
	dma_addr_t driver_event_dma_addr;
	dma_addr_t device_event_dma_addr;
	size_t ring_size_in_bytes;
	size_t event_size_in_bytes;
};

struct vring_virtqueue {
	struct virtqueue vq;
	bool packed_ring;
	bool use_dma_api;
	bool weak_barriers;
	bool broken;
	bool indirect;
	bool event;
	unsigned int free_head;
	unsigned int num_added;
	u16 last_used_idx;
	bool event_triggered;
	union {
		struct vring_virtqueue_split split;
		struct vring_virtqueue_packed packed;
	};
	bool (*notify)(struct virtqueue *);
	bool we_own_ring;
};