#  一般情况无需修改该文件

LOCAL_PATH=$(dir  $(abspath $(lastword $(MAKEFILE_LIST))))
ZKSW_PREBUILT_LIB_PATH := $(ZKSW_REAL_PREBUILT)/lib/
ZKSW_PREBUILT_INCLUDE_PATH := $(ZKSW_REAL_PREBUILT)/include

EASYUI_COMMON_INCLUDE := $(LOCAL_PATH)/include

#配置源文件目录
MY_FILES_PATH  :=./
#源文件后缀名
MY_FILES_SUFFIX := %.cpp %.c
  
# 递归遍历目录下的所有的文件 
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))  
  
# 获取相应的源文件
MY_ALL_FILES := $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*.*) )   
MY_ALL_FILES := $(MY_ALL_FILES:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)  
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(MY_ALL_FILES))   
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)  
  
# 去除字串的重复单词
define uniq =  
  $(eval seen :=)  
  $(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))  
  ${seen}  
endef  
 
#递归遍历目录下的所有的文件
MY_ALL_DIRS := $(dir $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*/) ) )  
MY_ALL_DIRS := $(call uniq,$(MY_ALL_DIRS))  
  
# 获取相应的源文件
LOCAL_SRC_FILES  := $(MY_SRC_LIST) 

CFLAGS=-Os -pipe -march=armv5te -mtune=arm926ej-s -fno-caller-saves -Wno-unused-result -mfloat-abi=soft  -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro       -Wl,--warn-common -Wl,--warn-once -Wl,-z,combreloc -Wl,-z,defs    -DUSE_ANDROID_LOG
CXXFLAGS=-Os -pipe -march=armv5te -mtune=arm926ej-s -fno-caller-saves -Wno-unused-result -mfloat-abi=soft  -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -Wno-virtual-dtor    -DUSE_ANDROID_LOG

LOCAL_CPPFLAGS := -fexceptions -fpermissive -Wall -O2 -luClibc++ -leasyui -llog -DOPENSSL -lssl -lcrypto
LOCAL_LIBS := -L$(ZKSW_PREBUILT_LIB_PATH)/system \
				-L.

LOCAL_INCLUDES := -I$(EASYUI_COMMON_INCLUDE) \
					-I$(ZKSW_PREBUILT_INCLUDE_PATH)/gui \
					-I$(ZKSW_PREBUILT_INCLUDE_PATH)/uClibc++ \
					-I$(ZKSW_PREBUILT_INCLUDE_PATH)/system \
					-I. \

TARGET=../libs/armeabi/libzkgui.so

# alias commads if compile on windows
UNAME := $(shell uname)
RM =rm
MKDIR=mkdir
ECHO=echo
ifeq ($(UNAME), windows32)
	RM=win32-rm
	MKDIR=win32-mkdir
	ECHO=win32-echo
else
	#keep default
endif
#alias end

all:$(TARGET)
	@$(ECHO)
	@$(ECHO) "[armeabi] Install        : libzkgui.so => libs/armeabi/libzkgui.so"
	
$(TARGET):$(LOCAL_SRC_FILES)
	@-$(MKDIR) -p ../libs/armeabi
	@-$(RM) $(TARGET) -rf
	$(CC) -fPIC -shared -o $(TARGET)  \
	$(LOCAL_INCLUDES) \
	$(LOCAL_SRC_FILES) \
	$(LOCAL_CPPFLAGS) \
	$(LOCAL_LIBS) \
	$(CFLAGS)

clean:
	-$(RM) ../libs -rf
.PHONY:all clean $(TARGET)
