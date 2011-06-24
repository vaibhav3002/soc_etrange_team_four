CC_PREFIX    = lm32-elf-
CC           = $(CC_PREFIX)gcc

TARGET_ARCH  = -mmultiply-enabled -mbarrel-shift-enabled
DEBUG_CFLAGS = -ggdb
LDFLAGS      = -Tsoclib.ld
CFLAGS       = -Wall -ggdb -O2 $(DEBUG_CFLAGS) 
CPPFLAGS     = -I.

SOFT_IMAGE = soft.elf

OBJS  =  main.o lm32_sys.o etrange.o mfixed.o logo_telecom.o

.PHONY: all clean

all:$(SOFT_IMAGE)

logo_telecom.o: logo_telecom.pgm
	lm32-elf-objcopy -I binary -O elf32-lm32 -B lm32 --rename-section .data=.rodata,alloc,load,readonly,data,contents --reverse-bytes=4 ./logo_telecom.pgm ./logo_telecom.o

$(SOFT_IMAGE):$(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^
clean:
	rm -f $(OBJS) $(SOFT_IMAGE)
