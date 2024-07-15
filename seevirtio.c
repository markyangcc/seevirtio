#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/virtio.h>
#include <linux/proc_fs.h>
#include <linux/virtio_config.h>
#include <uapi/linux/virtio_net.h>
#include <uapi/linux/virtio_ring.h>

#include "seevirtio.h"

extern struct net init_net;

MODULE_AUTHOR("me");
MODULE_DESCRIPTION("virtio-net see");
MODULE_LICENSE("GPL");

/*
 * Helpers.
 */
#define to_vvq(_vq) container_of(_vq, struct vring_virtqueue, vq)

static int seevirtio_proc_show(struct seq_file *m, void *v)
{
	struct net_device *ndev = (struct net_device *)(m->private);

	struct virtnet_info *vi = netdev_priv(ndev);

	seq_printf(m, "Queue \t\tInQueue \tVringNum \tLastUsed \tName\n");
	seq_printf(m, "-------------------------------------------------------------------------\n");

	for (int i = 0; i < vi->max_queue_pairs; i++) {
		struct virtqueue *_vq = vi->rq[i].vq;
		unsigned int qnum = _vq->index;

		struct vring_virtqueue *vq = to_vvq(_vq);
		int used = vq->split.vring.used->idx;
		int last_used = vq->last_used_idx;
		int vring_num = vq->split.vring.num;

		int rx_inqueue = (used - last_used) & (vq->split.vring.num - 1);
		seq_printf(m, "Rx %2d: \t\t%4d \t\t%4d \t\t%d \t\t%s\n", qnum, rx_inqueue, vring_num, last_used, _vq->name);
	}

	for (int i = 0; i < vi->max_queue_pairs; i++) {
		struct virtqueue *_vq = vi->sq[i].vq;
		unsigned int qnum = _vq->index;

		struct vring_virtqueue *vq = to_vvq(_vq);
		int avail = vq->split.vring.avail->idx;
		int used = vq->split.vring.used->idx;
		int last_used = vq->last_used_idx;
		int vring_num = vq->split.vring.num;

		int tx_inqueue = (avail - used) & (vq->split.vring.num - 1);
		seq_printf(m, "Tx %2d: \t\t%4d \t\t%4d \t\t%d \t\t%s\n", qnum, tx_inqueue, vring_num, last_used, _vq->name);
	}

	seq_printf(m, "\n");

	return 0;
}

static int __init seevirtio_init(void)
{
	struct proc_dir_entry *pde;
	struct net_device *dev;

	dev = dev_get_by_name(&init_net, "eth0");
	if (!dev) {
		pr_info("SeeVirtio: failed\n");
		return -ENODEV;
	}

	/* Create procfs entry */
	pde = proc_create_single_data("seevirtio", 0, NULL, seevirtio_proc_show, dev);
	if (!pde) {
		pr_warn("Unable to initialize seevirtio /proc entry\n");
		return -ENOMEM;
	}

	return 0;
}

static void __exit seevirtio_exit(void)
{
	remove_proc_entry("seevirtio", NULL);
}

module_init(seevirtio_init);
module_exit(seevirtio_exit);