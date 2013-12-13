CC = gcc
CC_OP = -g -o -Wall -Werror
CFLAGS = -I/usr/include/libxml2
LINK_LIB = -lxml2 -lz -lm -lpthread -lsctp
OBJS_M = sn_main.o sn_cli.o make_xml.o parse_xml.o namespace_server.o
OBJS =  utility.o xml_msg.o errmsg.o
ALL = sn_main sn_cli mk_xml_to_stdout parse_xml_from_stdin namespace_server
.PHONY : all sn_main sn_cli mk_xml_to_stdout parse_xml_from_stdin namespace_server
all : sn_main sn_cli xml_msg.o mk_xml_to_stdout parse_xml_from_stdin namespace_server
sn_main : sn_main.o xml_msg.o utility.o
	$(CC) sn_main.o xml_msg.o utility.o $(CC_OP) sn_main $(LINK_LIB)
sn_cli : sn_cli.o xml_msg.o utility.o
	$(CC) sn_cli.o xml_msg.o utility.o $(CC_OP) sn_cli $(LINK_LIB)
mk_xml_to_stdout : make_xml.o
	$(CC) make_xml.o $(CC_OP) mk_xml_to_stdout $(LINK_LIB)
parse_xml_from_stdin : parse_xml.o xml_msg.o utility.o
	$(CC) parse_xml.o xml_msg.o utility.o $(CC_OP) parse_xml_from_stdin $(LINK_LIB)
xml_msg.o :
	$(CC) $(CFLAGS) -c xml_msg.c
namespace_server : namespace_server.o utility.o errmsg.o 
	$(CC) namespace_server.o utility.o errmsg.o $(CC_OP) namespace_server $(LINK_LIB)
.PHONY : clean
clean : 
	rm $(OBJS_M) $(OBJS) $(ALL) user_info.txt
