##### Macros

CC = ccpsx

#CFLAGS = -c -g -O2 -comments-c++ -nostdlib -fsigned-char -mgpopt -G 0 -DALL -Wall
CFLAGS = -c -g  -comments-c++ -nostdlib -fsigned-char -mgpopt -G 0 -DALL -Wall

CFLAGSL = -c -O2 -comments-c++ -fsigned-char -mgpopt -G 0
# -Wall
#CFLAGSL = -c -g  -comments-c++ -fsigned-char -mgpopt -G 0
#-Wall

#LFLAGS = /c /i /m
LFLAGS = /c /d /m

OBJS1 = tstblk.obj


##### Build



tstblk: $(OBJS1)
	psylink $(LFLAGS) @$&.lnk,$&.cpe,$&.sym,$&.map







##### Build objects depending on wether debugging or not

tstblk.obj: tstblk.c

##### Implicit rules

.c.obj:
	$(CC) $(CFLAGS) $&.c -o $&.obj

.s.obj:
  asmpsx /l /x $&.s,$&.obj,$&.map,$&.sym


##### Clean

clean:
	del *.cpe
	del *.obj
	del *.obd
	del *.sym
	del *.map
