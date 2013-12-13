CC = gcc
CC_FLAGS = -g -o
LINK_LIBS = -lcrypto -lpthread
OBJS = dst.o src.o rsync_utility.o
ALL = src dst
.PHONY : all src dst
all : src dst
src : rsync_utility.o src.o 
	$(CC) rsync_utility.o src.o $(CC_FLAGS) src $(LINK_LIBS)
dst : rsync_utility.o dst.o
	$(CC) rsync_utility.o dst.o $(CC_FLAGS) dst $(LINK_LIBS)
.PHONY : clean
clean :
	rm	$(OBJS) $(ALL)
