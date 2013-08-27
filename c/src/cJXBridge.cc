/*
** Copyright (C) 2013 Mellanox Technologies
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at:
**
** http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
** either express or implied. See the License for the specific language
** governing permissions and  limitations under the License.
**
*/

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <jni.h>

#include <libxio.h>

#include "CallbackFunctions.h"
#include "cJXSession.h"
#include "cJXServer.h"

#include "cJXCtx.h"
#include "Utils.h"


static jclass cls;
static JavaVM *cached_jvm;


static jclass cls_data;

static jfieldID fidPtr;
static jfieldID fidBuf;
static jfieldID fidError;




// JNI inner functions implementations



extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void* reserved)
{
	printf("in cJXBridge - JNI_OnLoad\n");

	cached_jvm = jvm;
	JNIEnv *env;
	if ( jvm->GetEnv((void **)&env, JNI_VERSION_1_4)) { //direct buffer requires java 1.4
		return JNI_ERR; /* JNI version not supported */
	}

	cls = env->FindClass("com/mellanox/JXBridge");
	if (cls == NULL) {
		fprintf(stderr, "in cJXBridge - java class was NOT found\n");
		return JNI_ERR;
	}

	cls_data = env->FindClass("com/mellanox/EventQueueHandler$DataFromC");
	if (cls_data == NULL) {
			fprintf(stderr, "in cJXBridge - java class was NOT found\n");
			return JNI_ERR;
		}

	if (fidPtr == NULL) {
	fidPtr = env->GetFieldID(cls_data, "ptrCtx", "J");
	if (fidPtr == NULL) {
		fprintf(stderr, "could not get field  ptrCtx\n");
		}
	}

	if (fidBuf == NULL) {
	fidBuf = env->GetFieldID(cls_data, "eventQueue","Ljava/nio/ByteBuffer;");
	if (fidBuf == NULL) {
		fprintf(stderr, "could not get field  fidBuf\n");
		}
	}

	printf("in cJXBridge -  java callback methods were found and cached\n");

	return JNI_VERSION_1_4;  //direct buffer requires java 1.4

}



extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void* reserved)
{
	// NOTE: We never reached this place
	return;
}





extern "C" JNIEXPORT jboolean JNICALL Java_com_mellanox_JXBridge_createCtxNative(JNIEnv *env, jclass cls, jint eventQueueSize, jobject dataToC)
{
	int size = eventQueueSize;
	cJXCtx *ctx = new cJXCtx (size);
	if (ctx->errorCreating){
		delete (ctx);
		return false;
	}
	jobject jbuf = env->NewDirectByteBuffer(ctx->eventQueue->getBuffer(), eventQueueSize );

	jlong ptr = (jlong)(intptr_t) ctx;


	env->SetLongField(dataToC, fidPtr, ptr);
	env->SetObjectField(dataToC, fidBuf, jbuf);

	return ctx->errorCreating;
}


//Katya
extern "C" JNIEXPORT void JNICALL Java_com_mellanox_JXBridge_closeCtxNative(JNIEnv *env, jclass cls, jlong ptrCtx)
{
	cJXCtx *ctx = (cJXCtx *)ptrCtx;
	delete (ctx);
	printf("end of closeCTX\n");
}

//Katya
extern "C" JNIEXPORT jint JNICALL Java_com_mellanox_JXBridge_getNumEventsQNative(JNIEnv *env, jclass cls, jlong ptrCtx)
{
	cJXCtx *ctx = (cJXCtx *)ptrCtx;
	return ctx->eventsNum;
}



//Katya
extern "C" JNIEXPORT jint JNICALL Java_com_mellanox_JXBridge_runEventLoopNative(JNIEnv *env, jclass cls, jlong ptrCtx)
{
	cJXCtx *ctx = (cJXCtx *)ptrCtx;
	return ctx->runEventLoop();

}


//Katya
extern "C" JNIEXPORT void JNICALL Java_com_mellanox_JXBridge_stopEventLoopNative(JNIEnv *env, jclass cls, jlong ptrCtx)
{

	cJXCtx *ctx = (cJXCtx *)ptrCtx;
	ctx->stopEventLoop();
}



extern "C" JNIEXPORT jlong JNICALL Java_com_mellanox_JXBridge_startSessionClientNative(JNIEnv *env, jclass cls, jstring jurl, jlong ptrCtx) {

	const char *url = env->GetStringUTFChars(jurl, NULL);
	cJXSession * ses = new cJXSession(url, ptrCtx);
	env->ReleaseStringUTFChars(jurl, url);
	if (ses->errorCreating){
		delete (ses);
		return 0;
	}
	return (jlong)(intptr_t) ses;

}


//Katya
extern "C" JNIEXPORT jboolean JNICALL Java_com_mellanox_JXBridge_closeSessionClientNative(JNIEnv *env, jclass cls, jlong ptrSes)
{

	cJXSession * ses = (cJXSession*)ptrSes;
	return ses->closeConnection();

}



extern "C" JNIEXPORT jlongArray JNICALL Java_com_mellanox_JXBridge_startServerNative(JNIEnv *env, jclass cls, jstring jurl, jlong ptrCtx) {

	jlong temp[2];

	jlongArray dataToJava = env->NewLongArray(2);
	if (dataToJava == NULL) {
		return NULL; /* out of memory error thrown */
	}

	const char *url = env->GetStringUTFChars(jurl, NULL);

	cJXServer * server = new cJXServer(url, ptrCtx);
	env->ReleaseStringUTFChars(jurl, url);
	if (server->errorCreating){
		temp[0] = 0;
		temp[1]=0;
		delete(server);
	}else{
		temp [0] = (jlong)(intptr_t) server;
		temp[1] = (jlong)server->port;
	}

	env->SetLongArrayRegion(dataToJava,0, 2, temp);
	return dataToJava;

}


//Katya
extern "C" JNIEXPORT void JNICALL Java_com_mellanox_JXBridge_stopServerNative(JNIEnv *env, jclass cls, jlong ptrServer)
{
	cJXServer *server = (cJXServer *)ptrServer;
	delete server;

}




extern "C" JNIEXPORT jboolean JNICALL Java_com_mellanox_JXBridge_forwardSessionNative(JNIEnv *env, jclass cls, jstring jurl, jlong ptrSession) {

	struct xio_session	*session;	
	int retVal;


	const char *url = env->GetStringUTFChars(jurl, NULL);

	session = (struct xio_session *)ptrSession;
	
	retVal = xio_accept (session, &url, 1, NULL, 0);

//	retVal = xio_accept(session, NULL, 0, NULL, 0);

	env->ReleaseStringUTFChars(jurl, url);

    if (retVal){
    	log (lsERROR, "Error in accepting session. error %d\n", retVal);
		return false;
	}
	return true;
	
}


extern "C" JNIEXPORT jstring JNICALL Java_com_mellanox_JXBridge_getErrorNative(JNIEnv *env, jclass cls, jint errorReason) {

	struct xio_session	*session;
	const char * error;
	jstring str;

	error = xio_strerror(errorReason);
	str = env->NewStringUTF(error);
//	free(error); TODO: to free it????
	return str;

}







JNIEnv *JX_attachNativeThread()
{
    JNIEnv *env;
	if (! cached_jvm) {
		printf("cached_jvm is NULL");
	}
    jint ret = cached_jvm->AttachCurrentThread((void **)&env, NULL);

	if (ret < 0) {
		printf("cached_jvm->AttachCurrentThread failed ret=%d", ret);
	}
	printf("completed successfully env=%p", env);
    return env; // note: this handler is valid for all functions in this thread
}








