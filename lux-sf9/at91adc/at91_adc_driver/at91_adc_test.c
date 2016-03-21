#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define ADC_READ 1

int main(int argc, char **argv)
{
  int fd=-1, chan=0, val;

  if(argc<2){
    fprintf(stderr, "usage: adc-test DEVICE\n");
    exit(1);
  }

  if((fd=open(argv[1],O_RDONLY))<0){
    fprintf(stderr, "can't open %s\n", argv[1]);
    exit(1);
  }

  if(argc>2){ chan = atoi(argv[2]); }

  val=ioctl(fd,ADC_READ,chan);
  if(val<0){
    fprintf(stderr, "read error %d\n",val);
    exit(1);
  }

  printf("%d\n", val);
  close(fd);
    
  return 0;
}
