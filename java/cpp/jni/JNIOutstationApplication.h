//
//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// 
// This file is auto-generated. Do not edit manually
// 
// Copyright 2016 Automatak LLC
// 
// Automatak LLC (www.automatak.com) licenses this file
// to you under the the Apache License Version 2.0 (the "License"):
// 
// http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef OPENDNP3JAVA_JNIOUTSTATIONAPPLICATION_H
#define OPENDNP3JAVA_JNIOUTSTATIONAPPLICATION_H

#include <jni.h>

namespace jni
{
    struct JCache;

    namespace cache
    {
        class OutstationApplication
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // methods
            jboolean supportsAssignClass(JNIEnv* env, jobject instance);
            jboolean writeAbsoluteTime(JNIEnv* env, jobject instance, jlong arg0);
            jboolean supportsWriteAbsoluteTime(JNIEnv* env, jobject instance);
            void recordClassAssignment(JNIEnv* env, jobject instance, jobject arg0, jobject arg1, jint arg2, jint arg3);
            jobject getApplicationIIN(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // method ids
            jmethodID supportsAssignClassMethod = nullptr;
            jmethodID writeAbsoluteTimeMethod = nullptr;
            jmethodID supportsWriteAbsoluteTimeMethod = nullptr;
            jmethodID recordClassAssignmentMethod = nullptr;
            jmethodID getApplicationIINMethod = nullptr;
        };
    }
}

#endif
