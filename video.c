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

static void std_set (int fd, v4l2_std_id id)
{
   struct v4l2_input input;
   v4l2_std_id std_id;

   memset (&input, 0, sizeof (input));

   if (-1 == ioctl (fd, VIDIOC_G_INPUT, &input.index)) {
        perror ("VIDIOC_G_INPUT");
   }

   if (-1 == ioctl (fd, VIDIOC_ENUMINPUT, &input)) {
        perror ("VIDIOC_ENUM_INPUT");
   }

   if (0 == (input.std & id)) {
        fprintf (stderr, "Oops. STD is not supported.\n");
   }

   /* Note this is also supposed to work when only B
      or G/PAL is supported. */

   std_id = id;

   if (-1 == ioctl (fd, VIDIOC_S_STD, &std_id)) {
        perror ("VIDIOC_S_STD");
   }
}

static void current_std_get (int fd)
{
    v4l2_std_id std_id;
    struct v4l2_standard standard;
    int i;

    if (-1 == ioctl (fd, VIDIOC_G_STD, &std_id)) {
        /* Note when VIDIOC_ENUMSTD always returns EINVAL this
           is no video device or it falls under the USB exception,
           and VIDIOC_G_STD returning EINVAL is no error. */

        perror ("VIDIOC_G_STD");
    }

    memset (&standard, 0, sizeof (standard));
    i = 0;
    standard.index = i;

    while (0 == ioctl (fd, VIDIOC_ENUMSTD, &standard)) {
        if (standard.id & std_id) {
               printf ("Current video standard: %s\n", standard.name);
        }
	i++;
        standard.index = i;
    }

    /* EINVAL indicates the end of the enumeration, which cannot be
       empty unless this device falls under the USB exception. */

    if (errno != EINVAL || i == 0) {
        perror ("VIDIOC_ENUMSTD");
    }   
}

static void enumerate_std (int fd)
{
   struct v4l2_input input;
   struct v4l2_standard standard;
   int i;

   memset (&input, 0, sizeof (input));

   if (-1 == ioctl (fd, VIDIOC_G_INPUT, &input.index)) {
      perror ("VIDIOC_G_INPUT");
   }

   if (-1 == ioctl (fd, VIDIOC_ENUMINPUT, &input)) {
      perror ("VIDIOC_ENUM_INPUT");
   }

   printf ("Current input %s supports:\n", input.name);

   memset (&standard, 0, sizeof (standard));
   i = 0;
   standard.index = i;

   while (0 == ioctl (fd, VIDIOC_ENUMSTD, &standard)) {
        if (standard.id & input.std)
             printf ("%s\n", standard.name);

	i++;
        standard.index = i;
   }

   /* EINVAL indicates the end of the enumeration, which cannot be
         empty unless this device falls under the USB exception. */

   if (errno != EINVAL || i == 0) {
        perror ("VIDIOC_ENUMSTD");
   }
}

static void enumerate_menu (int fd, struct v4l2_queryctrl *queryctrl)
{
   struct v4l2_querymenu querymenu;

   printf ("  Menu items:\n");

        memset (&querymenu, 0, sizeof (querymenu));
        querymenu.id = queryctrl->id;

        for (querymenu.index = queryctrl->minimum;
             querymenu.index <= queryctrl->maximum;
              querymenu.index++) {
                if (0 == ioctl (fd, VIDIOC_QUERYMENU, &querymenu)) {
                        printf ("    %d %s\n", querymenu.id, querymenu.name);
                } else {
                        perror ("VIDIOC_QUERYMENU");
                        exit (EXIT_FAILURE);
                }
        }
}

static void 
buffer_setup(int fd)
{
   struct v4l2_requestbuffers reqbuf;
   struct {
        void *start;
        size_t length;
   } *buffers;
   unsigned int i;

   memset (&reqbuf, 0, sizeof (reqbuf));
   reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   reqbuf.memory = V4L2_MEMORY_MMAP;
   reqbuf.count = 20;

   if (-1 == ioctl (fd, VIDIOC_REQBUFS, &reqbuf)) {
        if (errno == EINVAL)
                printf ("Video capturing or mmap-streaming is not supported\n");
        else
                perror ("VIDIOC_REQBUFS");

        exit (EXIT_FAILURE);
   }

   /* We want at least five buffers. */

   if (reqbuf.count < 5) {
        /* You may need to free the buffers here. */
        printf ("Not enough buffer memory\n");
        exit (EXIT_FAILURE);
   }

   buffers = calloc (reqbuf.count, sizeof (*buffers));

   for (i = 0; i < reqbuf.count; i++) {
        struct v4l2_buffer buffer;

        memset (&buffer, 0, sizeof (buffer));
        buffer.type = reqbuf.type;
	buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buffer)) {
                perror ("VIDIOC_QUERYBUF");
                exit (EXIT_FAILURE);
        }

        buffers[i].length = buffer.length; /* remember for munmap() */

        buffers[i].start = mmap (NULL, buffer.length,
                                 PROT_READ | PROT_WRITE, /* required */
                                 MAP_SHARED,             /* recommended */
                                 fd, buffer.m.offset);

        if (buffers[i].start == MAP_FAILED) {
                /* You may need to unmap and free the so far
                   mapped buffers here. */
                perror ("mmap");
                exit (EXIT_FAILURE);
        }
   }
}

int main(int argc, char **argv) {
     int fd;
     struct v4l2_format format;
     struct v4l2_capability cap;
     struct v4l2_input input;
     struct v4l2_audio audio;
     struct v4l2_queryctrl queryctrl;

     int index;

     fd = open("/dev/video", O_RDWR | O_NONBLOCK);

     memset(&cap, 0, sizeof(cap));
     ioctl(fd, VIDIOC_QUERYCAP, &cap);
     printf("Driver: %s, %s, 0x%x\n", cap.driver, cap.card, cap.capabilities);

     ioctl(fd, VIDIOC_G_INPUT, &index);

     printf("Inputs\n");
     while (1)
       {
	  struct v4l2_standard std;

	  memset (&std, 0, sizeof (std));
	  memset (&input, 0, sizeof (input));
	  input.index = index;

	  if (-1 == ioctl (fd, VIDIOC_ENUMINPUT, &input)) {
	       break;
	  }

	  index++;
	  printf ("Current input: %s 0x%x\n", input.name, input.std);
	 
	  if (input.type == V4L2_INPUT_TYPE_TUNER)
	    {
	       struct v4l2_tuner tuner;

	       memset (&tuner, 0, sizeof (tuner));
	       tuner.index = input.tuner;

	       ioctl(fd, VIDIOC_G_TUNER, &tuner);
	       printf(" Tuner: %s, 0x%x\n", tuner.name, tuner.capability);
	       printf(" Tuner: Frequency Range( %d -> %d )\n", tuner.rangelow, tuner.rangehigh);
	    }
	
	  

	  if (0 == ioctl (fd, VIDIOC_S_STD, &std))
	  if (0 == ioctl (fd, VIDIOC_G_STD, &std))
	     printf ("  Current Standard: %s perf(%d/%d)\n", std.name, std.frameperiod.numerator, std.frameperiod.denominator);
       }   

     printf("Driver Format\n");
     memset(&format, 0, sizeof(format));

     format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     ioctl(fd, VIDIOC_G_FMT, &format);
     printf("Size %d x %d\n", format.fmt.pix.width, format.fmt.pix.height);
     printf("Format: 0x%x\n", format.fmt.pix.pixelformat);
       {
	  int fmt = format.fmt.pix.pixelformat;
	  printf("Format Chars: %c %c %c %c\n", fmt & 0x00ff, 
		(fmt & 0x00ff00) >> 8,
		(fmt & 0x00ff0000) >> 16,
		(fmt & 0xff000000) >> 24);
       }

     printf("Audio Input\n");
     audio.index = 0;

     memset(&audio, 0, sizeof(audio));
     ioctl(fd, VIDIOC_ENUMAUDIO, &audio);
     printf("  Input: %s\n", audio.name);


printf("Controls:\n");
memset (&queryctrl, 0, sizeof (queryctrl));

for (index = V4L2_CID_BASE;
     index < V4L2_CID_LASTP1;
     index++) {

     queryctrl.id = index;
        if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
                if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                        continue;

                printf ("  Control %s: min(%d), max(%d), step(%d)\n", queryctrl.name, 
		      queryctrl.minimum, queryctrl.maximum, queryctrl.step);

                if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                        enumerate_menu (fd, &queryctrl);
        } else {
	     break;
	}
}

printf("Private Controls:\n");
for (index = V4L2_CID_PRIVATE_BASE;;
     index++) {

     queryctrl.id = index;

        if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
                if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                        continue;

                printf ("  Control %s: min(%d), max(%d), step(%d)\n", queryctrl.name,
		        queryctrl.minimum, queryctrl.maximum, queryctrl.step);

                if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                        enumerate_menu (fd, &queryctrl);
        } else {
	     break;
	}
}

struct v4l2_control ctl;
memset(&ctl, 0, sizeof (ctl));

ctl.id = V4L2_CID_AUDIO_MUTE;

ioctl(fd, VIDIOC_G_CTRL, &ctl);

printf("Mute Status = %d\n", ctl.value);

buffer_setup(fd);
current_std_get(fd);
enumerate_std(fd);
std_set(fd, V4L2_STD_NTSC_M_JP);

     close(fd);
}
