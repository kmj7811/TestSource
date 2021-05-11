#--------------------------------------------------------------
#               Quectel OpenLinux
#--------------------------------------------------------------
#QL_SDK_PATH   ?= $(shell pwd)/../..
QL_SDK_PATH   ?= /home/kim/tempev/1026/ql-ol-sdk/ql-ol-extsdk
#QL_SDK_PATH   ?= /home/brian/obigo-sdk/sdk/quectel-ag35e/arm-oe-linux-gnueabi

#QL_EXP_TARGETS = svclayer_nadifmgr.so
QL_EXP_TARGETS = libnadifmgr.so

QL_EXP_LDLIBS  = -lql_sys_log

#--------------------------------------------------------
# TOOLS BASIC CONFIG
# Note: No Need to change them
#--------------------------------------------------------
CPPFLAGS += -I./                     \
            -I./inc                  \
            -I../inc                 \
            -I$(QL_SDK_PATH)/include          \
            -I$(QL_SDK_PATH)/lib/interface/inc        \
            -I$(SDKTARGETSYSROOT)/usr/include               \
            -I$(SDKTARGETSYSROOT)/usr/include/data          \
            -I$(SDKTARGETSYSROOT)/usr/include/dsutils       \
            -I$(SDKTARGETSYSROOT)/usr/include/qmi           \
            -I$(SDKTARGETSYSROOT)/usr/include/qmi-framework \
            -I$(SDKTARGETSYSROOT)/usr/include/quectel-openlinux-sdk \



LDFLAGS += -L./                          \
           ${QL_EXP_LDLIBS}              \
           -L$(SDKTARGETSYSROOT)/usr/lib \
           -L$(QL_SDK_PATH)/lib          \
           -lpthread -lrt


STD_LIB= $(SDKTARGETSYSROOT)/usr/lib/libdsi_netctrl.so     \
         $(SDKTARGETSYSROOT)/usr/lib/libdsutils.so         \
         $(SDKTARGETSYSROOT)/usr/lib/libqmiservices.so     \
         $(SDKTARGETSYSROOT)/usr/lib/libqmi_cci.so         \
         $(SDKTARGETSYSROOT)/usr/lib/libqmi_common_so.so   \
         $(SDKTARGETSYSROOT)/usr/lib/libqmi.so             \
         $(SDKTARGETSYSROOT)/usr/lib/libmcm.so             \


#USR_LIB=$(QL_SDK_PATH)/lib/libql_common_api.a
#USR_LIB+=$(QL_SDK_PATH)/lib/libql_peripheral.a
USR_LIB=$(SDKTARGETSYSROOT)/usr/lib/libql_common_api.a
USR_LIB+=$(SDKTARGETSYSROOT)/usr/lib/libql_peripheral.a

LDFLAGS+=-shared
#---------------------
# Source code files
#---------------------
LOCAL_SRC_FILES = svclayer_NadCallMgr.c \

libnadifmgr.so: 
	$(COMPILE.c) $(CPPFLAGS) $(LDFLAGS) $(LOCAL_SRC_FILES)
	$(LINK.o) *.o $(LDFLAGS) $(USR_LIB) $(STD_LIB) -o $@

all: $(QL_EXP_TARGETS)

.PHPNY: all

clean:
	rm -rf $(QL_EXP_TARGETS) *.o

.PHONY:checkmake
checkmake:  
	@echo -e "CURDIR =		\n	${CURDIR}"  
	@echo -e "\nMAKE_VERSION =	\n	${MAKE_VERSION}"  
	@echo -e "\nMAKEFILE_LIST =	\n	${MAKEFILE_LIST}"  
	@echo -e "\nCOMPILE.c =		\n	${COMPILE.c}"
	@echo -e "\nCOMPILE.cc =	\n	${COMPILE.cc}"
	@echo -e "\nCOMPILE.cpp =	\n	${COMPILE.cpp}"
	@echo -e "\nLINK.cc =		\n	${LINK.cc}"
	@echo -e "\nLINK.o =		\n	${LINK.o}"
	@echo -e "\nCPPFLAGS =		\n	${CPPFLAGS}"
	@echo -e "\nCFLAGS =		\n	${CFLAGS}"
	@echo -e "\nCXXFLAGS =		\n	${CXXFLAGS}"
	@echo -e "\nLDFLAGS =		\n	${LDFLAGS}"
	@echo -e "\nLDLIBS =		\n	${LDLIBS}"
