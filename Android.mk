LOCAL_PATH :=$(call my-dir)

SNPE_LIB_DIR :=/home/ly/workspace/snpe-sdk2/lib/arm-android-gcc4.9
SNPE_INCLUDE_DIR :=/home/ly/workspace/snpe-sdk2/include/zdl

include $(CLEAR_VARS)
LOCAL_MODULE := FristJni
LOCAL_SRC_FILES :=com_example_myapplication1_FristJni.cpp  CheckRuntime.cpp LoadContainer.cpp LoadInputTensor.cpp SetBuilderOptions.cpp Util.cpp NV21Load.cpp udlExample.cpp CreateUserBuffer.cpp PreprocessInput.cpp SaveOutputTensor.cpp CreateGLBuffer.cpp
LOCAL_SHARED_LIBRARIES :=libSNPE libSYMPHONYCPU libSYMPHONYPOWER
LOCAL_LDLIBS := -lGLESv2 -lEGL -lm -llog
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libSNPE
LOCAL_SRC_FILES := $(SNPE_LIB_DIR)/libSNPE.so
LOCAL_EXPORT_C_INCLUDES += $(SNPE_INCLUDE_DIR)
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libSYMPHONYCPU
LOCAL_SRC_FILES := $(SNPE_LIB_DIR)/libsymphony-cpu.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libSYMPHONYPOWER
LOCAL_SRC_FILES := $(SNPE_LIB_DIR)/libsymphonypower.so
include $(PREBUILT_SHARED_LIBRARY)
