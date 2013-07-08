SENDER_BOOST = sender_boost
RECEIVER_BOOST = receiver_boost

SENDER_RAW = sender_raw
RECEIVER_RAW = receiver_raw

TARGET = $(SENDER_BOOST) $(SENDER_RAW) $(RECEIVER_BOOST) $(RECEIVER_RAW)

OBJECT_FILES_FOR_SENDER_BOOST = sender_boost.o
OBJECT_FILES_FOR_RECEIVER_BOOST = receiver_boost.o
OBJECT_FILES_FOR_SENDER_RAW = sender_raw.o
OBJECT_FILES_FOR_RECEIVER_02 = receiver_raw.o

OBJECT_FILES_COMMON =

LIB_DIR = /usr/local/lib

CXXFLAGS = -Wall -c -g

LDFLAGS = -L$(LIB_DIR) 
LDFLAGS += -levent -lboost_thread -lboost_filesystem -lpthread
LDFLAGS += -lboost_system
LDFLAGS += -Wl,-E

CC = g++

.PHONY: all clean

all: $(TARGET)

$(SENDER_BOOST): $(OBJECT_FILES_FOR_SENDER_BOOST) $(OBJECT_FILES_COMMON)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SENDER_RAW): $(OBJECT_FILES_FOR_SENDER_RAW) $(OBJECT_FILES_COMMON)
	$(CC) -o $@ $^ $(LDFLAGS)

$(RECEIVER_BOOST): $(OBJECT_FILES_FOR_RECEIVER_BOOST) $(OBJECT_FILES_COMMON)
	$(CC) -o $@ $^ $(LDFLAGS)

$(RECEIVER_RAW): $(OBJECT_FILES_FOR_RECEIVER_02) $(OBJECT_FILES_COMMON)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CXXFLAGS) $*.c -o $*.o
	$(CC) $(CXXFLAGS) -MM $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

%.o : %.cpp
	$(CC) $(CXXFLAGS) $*.cpp -o $*.o
	$(CC) $(CXXFLAGS) -MM $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

-include $(OBJECT_FILES:.o=.d)
	
clean:
	rm -rf $(TARGET) *.o *.d
