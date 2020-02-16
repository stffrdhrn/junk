#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <linux/videodev.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <sys/mman.h>
#define EXIT_FAILURE 1

#define CLR(b) memset(&b, 0, sizeof(b))

int main(int argc, char **argv) {
     int fd;
     struct v4l2_frequency freq;
     v4l2_std_id std;

     fd = open("/dev/video", O_RDWR | O_NONBLOCK);

     /* Get current channel */
     CLR(freq);
     if (0 == ioctl(fd, VIDIOC_G_FREQUENCY, &freq)) 
       {
	  printf("Current frequency : %d\n", freq.frequency);
       }

     /* Get current video standard */
     if (0 == ioctl(fd, VIDIOC_G_STD, &std))
       {
	  printf("Current standard : %x\n", std);
       }

     close(fd);
}
