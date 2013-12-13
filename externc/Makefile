CC = gcc
GPP = g++
OBJS = sb.o sbcpp.o extc.o extcpp.o
ALL = extc_c extcpp_c extc_cpp extcpp_cpp
.PHONY : all extc_c extcpp_c extc_cpp extcpp_cpp
all : extc_c extcpp_c extc_cpp extcpp_cpp
extc_c : extc.o sb.o
	$(CC) extc.o sb.o -o extc_c
extcpp_c : extcpp.o sb.o
	$(GPP) extcpp.o sb.o -o extcpp_c
extc_cpp : extc.o sbcpp.o
	$(GPP) extc.o sbcpp.o -o extc_cpp
extcpp_cpp : extcpp.o sbcpp.o
	$(GPP) extcpp.o sbcpp.o -o extcpp_cpp
.PHONY : clean
clean :
	rm $(ALL) $(OBJS)

