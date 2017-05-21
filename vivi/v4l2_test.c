#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>           
#include <fcntl.h>            
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>        
#include <linux/videodev2.h>

#define CAMERA_DEVICE "/dev/video0"
#define CAPTURE_FILE "frame_640x360.yuv"

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 360
#define VIDEO_FORMAT V4L2_PIX_FMT_YUYV
#define BUFFER_COUNT 4

typedef struct VideoBuffer {
    void   *start;
    size_t  length;
} VideoBuffer;


VideoBuffer framebuf[BUFFER_COUNT]; 


int main()
{
    int i, ret;

    int fd;
    fd = open(CAMERA_DEVICE, O_RDWR, 0);
    if (fd < 0) {
        printf("Open %s failed\n", CAMERA_DEVICE);
        return -1;
    }

    struct v4l2_capability cap;
    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        printf("VIDIOC_QUERYCAP failed (%d)\n", ret);
        return ret;
    }

    // Print capability infomations
    printf("Capability Informations:\n"); 
    printf(" driver: %s\n", cap.driver); // vivid
    printf(" card: %s\n", cap.card); // vivid
    printf(" bus_info: %s\n", cap.bus_info); //  platform:vivid-000
    printf(" version: %08X\n", cap.version); // 00040A00
    printf(" capabilities: %08X\n", cap.capabilities); // 853F0DF7

	struct v4l2_fmtdesc fmtdesc;
	for (i = 0;; i++) 
	{
		memset(&fmtdesc,0,sizeof(fmtdesc));
		fmtdesc.index = i;
		fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc))
			break;
		printf("	VIDIOC_ENUM_FMT(%d,VIDEO_CAPTURE)\n",i);
	}


    struct v4l2_format fmt;
 
    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = VIDEO_WIDTH;
    fmt.fmt.pix.height      = VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (ret < 0) {
        printf("VIDIOC_S_FMT failed (%d)\n", ret);
        return ret;
    }
    
    ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
    if (ret < 0) {
        printf("VIDIOC_G_FMT failed (%d)\n", ret);
        return ret;
    }
    // Print Stream Format
    printf("Stream Format Informations:\n");
    printf(" type: %d\n", fmt.type);
    printf(" width: %d\n", fmt.fmt.pix.width);
    printf(" height: %d\n", fmt.fmt.pix.height);
    char fmtstr[8];
    memset(fmtstr, 0, 8);
    memcpy(fmtstr, &fmt.fmt.pix.pixelformat, 4);
    printf(" pixelformat: %s\n", fmtstr);
    printf(" field: %d\n", fmt.fmt.pix.field);
    printf(" bytesperline: %d\n", fmt.fmt.pix.bytesperline);
    printf(" sizeimage: %d\n", fmt.fmt.pix.sizeimage);
    printf(" colorspace: %d\n", fmt.fmt.pix.colorspace);
    printf(" priv: %d\n", fmt.fmt.pix.priv);
    printf(" raw_date: %s\n", fmt.fmt.raw_data);

    struct v4l2_requestbuffers reqbuf;
    
    reqbuf.count = BUFFER_COUNT;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    
    ret = ioctl(fd , VIDIOC_REQBUFS, &reqbuf);
    if(ret < 0) {
        printf("VIDIOC_REQBUFS failed (%d)\n", ret);
        return ret;
    }

    VideoBuffer*  buffers = calloc( reqbuf.count, sizeof(*buffers) );
    struct v4l2_buffer buf;

    for (i = 0; i < reqbuf.count; i++) 
    {
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd , VIDIOC_QUERYBUF, &buf);
        if(ret < 0) {
            printf("VIDIOC_QUERYBUF (%d) failed (%d)\n", i, ret);
            return ret;
        }

        // mmap buffer
        framebuf[i].length = buf.length;
        framebuf[i].start = (char *) mmap(0, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (framebuf[i].start == MAP_FAILED) {
            printf("mmap (%d) failed: %s\n", i, strerror(errno));
            return -1;
        }
    
        // Queen buffer
        ret = ioctl(fd , VIDIOC_QBUF, &buf);
        if (ret < 0) {
            printf("VIDIOC_QBUF (%d) failed (%d)\n", i, ret);
            return -1;
        }

        printf("Frame buffer %d: address=0x%x, length=%d\n", i, (unsigned int)framebuf[i].start, framebuf[i].length);
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        printf("VIDIOC_STREAMON failed (%d)\n", ret);
        return ret;
    }

    // Get frame
    ret = ioctl(fd, VIDIOC_DQBUF, &buf);
    if (ret < 0) {
        printf("VIDIOC_DQBUF failed (%d)\n", ret);
        return ret;
    }

    // Process the frame
    FILE *fp = fopen(CAPTURE_FILE, "wb");
    if (fp < 0) {
        printf("open frame data file failed\n");
        return -1;
    }
    fwrite(framebuf[buf.index].start, 1, buf.length, fp);
    fclose(fp);
    printf("Capture one frame saved in %s\n", CAPTURE_FILE);

    // Re-queen buffer
    ret = ioctl(fd, VIDIOC_QBUF, &buf);
    if (ret < 0) {
        printf("VIDIOC_QBUF failed (%d)\n", ret);
        return ret;
    }

    // Release the resource
    for (i=0; i< 4; i++) 
    {
        munmap(framebuf[i].start, framebuf[i].length);
    }

    close(fd);
    printf("Camera test Done.\n");
    return 0;
}
