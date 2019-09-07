NDK_TOOLCHAIN_VERSION := 4.9
APP_ABI := armeabi-v7a
APP_OPTIM := release
APP_CFLAGS := -std=gnu++11
APP_CPPFLAGS += -std=gnu++11 -frtti -fexceptions
LOCAL_LDLIBS ：= -lm -llog -lz -dl
APP_LDFLAGS = -nodefaultlibs -lc -lm -ldl -lgcc
APP_STL := gnustl_shared

