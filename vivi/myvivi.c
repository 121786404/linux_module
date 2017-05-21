/********************************
* Module:       V4L2Dev         *
* Author:       Josh Chen       *
* Date:         2016/09/08      *
********************************/

#include <linux/module.h>
#include <linux/videodev2.h>
#include <media/videobuf2-vmalloc.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-event.h>
#include <media/v4l2-common.h>

#define IMG_WIDTH	320
#define IMG_HEIGHT	240
#define DEBUG 1
#ifdef DEBUG
#define DEBUG_PRINT(format, args...) printk("[%s:%d] "format, __FUNCTION__, __LINE__, ##args)
#else
#define DEBUG_PRINT(args...)
#endif

static struct v4l2_device my_vivi_v4l2_dev;
static struct mutex my_vivi_lock;

static struct v4l2_pix_format my_vivi_v4l2dev_video_format =
{
	.pixelformat	= V4L2_PIX_FMT_GREY,
	.width			= IMG_WIDTH,
	.height			= IMG_HEIGHT,
	.field			= V4L2_FIELD_NONE,
	.colorspace		= V4L2_COLORSPACE_SRGB,
	.bytesperline	= IMG_WIDTH,
	.sizeimage		= IMG_WIDTH * IMG_HEIGHT,
};

static int my_vivi_v4l2dev_vidioc_querycap(struct file *file, void *priv, struct v4l2_capability *cap)
{
	DEBUG_PRINT("\n");
	strlcpy(cap->driver, "v4l2dev", sizeof(cap->driver));
	strlcpy(cap->card, "V4L2Dev Device", sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info), "V4L2Dev Bus");
	cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int my_vivi_v4l2dev_vidioc_enum_fmt(struct file *file, void *priv, struct v4l2_fmtdesc *f)
{
	DEBUG_PRINT("\n");
	if (f->index != 0)
	{
		DEBUG_PRINT("v4l2_fmtdesc index != 0\n");
		return -EINVAL;
	}
	f->pixelformat = V4L2_PIX_FMT_GREY;
	return 0;
}

static int my_vivi_v4l2dev_vidioc_fmt(struct file *file, void *priv, struct v4l2_format *f)
{
	DEBUG_PRINT("\n");
	f->fmt.pix = my_vivi_v4l2dev_video_format;
	return 0;
}

static int my_vivi_v4l2dev_vidioc_enum_input(struct file *file, void *priv, struct v4l2_input *i)
{
	DEBUG_PRINT("\n");
	if (i->index != 0)
	{
		DEBUG_PRINT("v4l2_input index != 0\n");
		return -EINVAL;
	}
	i->type = V4L2_INPUT_TYPE_CAMERA;
	strlcpy(i->name, "V4L2Dev", sizeof(i->name));
	return 0;
}

static int my_vivi_v4l2dev_vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	DEBUG_PRINT("\n");
	return (i == 0) ? 0 : -EINVAL;
}

static int my_vivi_v4l2dev_vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	DEBUG_PRINT("\n");
	*i = 0;
	return 0;
}

  
static int my_vivi_v4l2dev_queue_setup(struct vb2_queue *q, unsigned int *nbuffers, unsigned int *nplanes, unsigned int sizes[], struct device *alloc_ctxs[])	//for > 4.4
{
	size_t size = IMG_WIDTH * IMG_HEIGHT;
	DEBUG_PRINT("\n");

	if (*nplanes)
		return sizes[0] < size ? -EINVAL : 0;
	*nplanes = 1;
	sizes[0] = size;
	return 0;
}

static void my_vivi_v4l2dev_buffer_queue(struct vb2_buffer *vb)
{
	static unsigned char data = 1;
	u8 *ptr;

	DEBUG_PRINT("\n");

	ptr = vb2_plane_vaddr(vb, 0);
	if (!ptr)
	{
		DEBUG_PRINT("vb2_plane_vaddr fail\n");
		goto fault;
	}

	memset(ptr, data++, IMG_WIDTH * IMG_HEIGHT);

	vb2_set_plane_payload(vb, 0, IMG_WIDTH * IMG_HEIGHT);
	vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
	return;
fault:
	vb2_buffer_done(vb, VB2_BUF_STATE_ERROR);
}

static struct vb2_ops my_vivi_v4l2dev_queue_ops = {
	.queue_setup	= my_vivi_v4l2dev_queue_setup,
	.buf_queue		= my_vivi_v4l2dev_buffer_queue,
	.wait_prepare	= vb2_ops_wait_prepare,
	.wait_finish	= vb2_ops_wait_finish,
};

static struct vb2_queue my_vivi_v4l2dev_queue = {
	.type				= V4L2_BUF_TYPE_VIDEO_CAPTURE,
	.io_modes			= VB2_MMAP | VB2_READ,
	.buf_struct_size	= sizeof(struct vb2_buffer),
	.ops				= &my_vivi_v4l2dev_queue_ops,
	.mem_ops			= &vb2_vmalloc_memops,
	.timestamp_flags	= V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC,
	.min_buffers_needed	= 1,
};

static struct v4l2_file_operations my_vivi_v4l2dev_video_fops = {
	.owner			= THIS_MODULE,
	.open			= v4l2_fh_open,
	.release		= vb2_fop_release,
	.unlocked_ioctl	= video_ioctl2,
	.read			= vb2_fop_read,
	.mmap			= vb2_fop_mmap,
	.poll			= vb2_fop_poll,
};

static struct v4l2_ioctl_ops my_vivi_v4l2dev_video_ioctl_ops = {
	.vidioc_querycap			= my_vivi_v4l2dev_vidioc_querycap,
	.vidioc_enum_fmt_vid_cap	= my_vivi_v4l2dev_vidioc_enum_fmt,
	.vidioc_try_fmt_vid_cap		= my_vivi_v4l2dev_vidioc_fmt,
	.vidioc_s_fmt_vid_cap		= my_vivi_v4l2dev_vidioc_fmt,
	.vidioc_g_fmt_vid_cap		= my_vivi_v4l2dev_vidioc_fmt,
 	.vidioc_enum_input			= my_vivi_v4l2dev_vidioc_enum_input,
	.vidioc_g_input				= my_vivi_v4l2dev_vidioc_g_input,
	.vidioc_s_input				= my_vivi_v4l2dev_vidioc_s_input,
	.vidioc_reqbufs				= vb2_ioctl_reqbufs,
	.vidioc_create_bufs			= vb2_ioctl_create_bufs,
	.vidioc_querybuf			= vb2_ioctl_querybuf,
	.vidioc_qbuf				= vb2_ioctl_qbuf,
	.vidioc_dqbuf				= vb2_ioctl_dqbuf,
	.vidioc_expbuf				= vb2_ioctl_expbuf,
	.vidioc_streamon			= vb2_ioctl_streamon,
	.vidioc_streamoff			= vb2_ioctl_streamoff,
};

static struct video_device my_vivi_v4l2dev_video_device = {
	.name		= "V4L2Dev Video Device",
	.fops		= &my_vivi_v4l2dev_video_fops,
	.ioctl_ops	= &my_vivi_v4l2dev_video_ioctl_ops,
	.release	= video_device_release_empty,
};

static int __init myvivi_init(void)
{    
	int ret;
	DEBUG_PRINT("module inserte begin\n");
	
	strlcpy(my_vivi_v4l2_dev.name, "Myvivi V4L2 Device", sizeof(my_vivi_v4l2_dev.name));
	ret = v4l2_device_register(NULL, &my_vivi_v4l2_dev);
	if (ret < 0)
	{
		DEBUG_PRINT("v4l2_device_register fail\n");
		goto error_unreg_v4l2;
	}

	mutex_init(&my_vivi_lock);
	my_vivi_v4l2dev_queue.lock	= &my_vivi_lock;
	ret = vb2_queue_init(&my_vivi_v4l2dev_queue);
	if (ret < 0)
	{
		DEBUG_PRINT("vb2_queue_init fail\n");
		goto error_unreg_v4l2;
	}
	DEBUG_PRINT("vb2_queue_init success!\n");

	strlcpy(my_vivi_v4l2dev_video_device.name, "Myvivi Video Device", sizeof(my_vivi_v4l2dev_video_device.name));
	my_vivi_v4l2dev_video_device.v4l2_dev	= &my_vivi_v4l2_dev;
	my_vivi_v4l2dev_video_device.lock		= &my_vivi_lock;
	my_vivi_v4l2dev_video_device.queue	= &my_vivi_v4l2dev_queue;
	video_set_drvdata(&my_vivi_v4l2dev_video_device, &my_vivi_v4l2_dev);

	ret = video_register_device(&my_vivi_v4l2dev_video_device, VFL_TYPE_GRABBER, -1);
	if (ret < 0)
	{
		DEBUG_PRINT("video_register_device fail\n");
		ret = -ENODEV;
		goto error_unreg_video;
	}
	DEBUG_PRINT("module inserte done\n");
	return 0;
	
	DEBUG_PRINT("module inserte fail!!\n");
error_unreg_video:
	video_unregister_device(&my_vivi_v4l2dev_video_device);
error_unreg_v4l2:
	v4l2_device_unregister(&my_vivi_v4l2_dev);
	return ret;
}

static void __exit myvivi_exit(void)
{
	DEBUG_PRINT("module remove begin\n");
	video_unregister_device(&my_vivi_v4l2dev_video_device);
	v4l2_device_unregister(&my_vivi_v4l2_dev);
	DEBUG_PRINT("module remove done\n");
}

module_init(myvivi_init);
module_exit(myvivi_exit);

MODULE_DESCRIPTION("Myvivi Device");
MODULE_LICENSE("GPL");

