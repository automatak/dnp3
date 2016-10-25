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

#include "JNIStack.h"

namespace jni
{
    namespace cache
    {
        bool Stack::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/Stack;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->shutdownMethod = env->GetMethodID(this->clazz, "shutdown", "()V");
            if(!this->shutdownMethod) return false;

            this->disableMethod = env->GetMethodID(this->clazz, "disable", "()V");
            if(!this->disableMethod) return false;

            this->enableMethod = env->GetMethodID(this->clazz, "enable", "()V");
            if(!this->enableMethod) return false;

            return true;
        }

        void Stack::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        void Stack::shutdown(JNIEnv* env, jobject instance)
        {
            env->CallVoidMethod(instance, this->shutdownMethod);
        }

        void Stack::disable(JNIEnv* env, jobject instance)
        {
            env->CallVoidMethod(instance, this->disableMethod);
        }

        void Stack::enable(JNIEnv* env, jobject instance)
        {
            env->CallVoidMethod(instance, this->enableMethod);
        }
    }
}
