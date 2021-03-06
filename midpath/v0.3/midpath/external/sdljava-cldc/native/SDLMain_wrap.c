/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.22
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */


#if defined(__GNUC__)
    typedef long long __int64; /*For gcc on Windows */
#endif
#include <jni.h>
#include <stdlib.h>
#include <string.h>


/* Support for throwing Java exceptions */
typedef enum {
  SWIG_JavaOutOfMemoryError = 1, 
  SWIG_JavaIOException, 
  SWIG_JavaRuntimeException, 
  SWIG_JavaIndexOutOfBoundsException,
  SWIG_JavaArithmeticException,
  SWIG_JavaIllegalArgumentException,
  SWIG_JavaNullPointerException,
  SWIG_JavaDirectorPureVirtual,
  SWIG_JavaUnknownError
} SWIG_JavaExceptionCodes;

typedef struct {
  SWIG_JavaExceptionCodes code;
  const char *java_exception;
} SWIG_JavaExceptions_t;


static void SWIG_JavaThrowException(JNIEnv *jenv, SWIG_JavaExceptionCodes code, const char *msg) {
  jclass excep;
  static const SWIG_JavaExceptions_t java_exceptions[] = {
    { SWIG_JavaOutOfMemoryError, "java/lang/OutOfMemoryError" },
    { SWIG_JavaIOException, "java/io/IOException" },
    { SWIG_JavaRuntimeException, "java/lang/RuntimeException" },
    { SWIG_JavaIndexOutOfBoundsException, "java/lang/IndexOutOfBoundsException" },
    { SWIG_JavaArithmeticException, "java/lang/ArithmeticException" },
    { SWIG_JavaIllegalArgumentException, "java/lang/IllegalArgumentException" },
    { SWIG_JavaNullPointerException, "java/lang/NullPointerException" },
    { SWIG_JavaDirectorPureVirtual, "java/lang/RuntimeException" },
    { SWIG_JavaUnknownError,  "java/lang/UnknownError" },
    { (SWIG_JavaExceptionCodes)0,  "java/lang/UnknownError" } };
  const SWIG_JavaExceptions_t *except_ptr = java_exceptions;

  while (except_ptr->code != code && except_ptr->code)
    except_ptr++;

  (*jenv)->ExceptionClear(jenv);
  excep = (*jenv)->FindClass(jenv, except_ptr->java_exception);
  if (excep)
    (*jenv)->ThrowNew(jenv, excep, msg);
}


/* Contract support */

#define SWIG_contract_assert(nullreturn, expr, msg) if (!(expr)) {SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, msg); return nullreturn; } else


#include "SDL.h"
#include "SDL_error.h"

  SDL_version SWIG_SDL_VERSION() {
    SDL_version version;
    
    SDL_VERSION(&version);

    return version;
  }

extern int SDL_Init(Uint32);
extern int SDL_InitSubSystem(Uint32);
extern void SDL_QuitSubSystem(Uint32);
extern Uint32 SDL_WasInit(Uint32);
extern void SDL_Quit(void);
extern SDL_version SWIG_SDL_VERSION();

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_set_1SDL_1version_1major(JNIEnv *jenv, jclass jcls, jlong jarg1, jshort jarg2) {
    SDL_version *arg1 = (SDL_version *) 0 ;
    Uint8 arg2 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(SDL_version **)&jarg1; 
    arg2 = (Uint8)jarg2; 
    if (arg1) (arg1)->major = arg2;
    
}


JNIEXPORT jshort JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_get_1SDL_1version_1major(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    SDL_version *arg1 = (SDL_version *) 0 ;
    Uint8 result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(SDL_version **)&jarg1; 
    result = (Uint8) ((arg1)->major);
    
    jresult = (jshort)result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_set_1SDL_1version_1minor(JNIEnv *jenv, jclass jcls, jlong jarg1, jshort jarg2) {
    SDL_version *arg1 = (SDL_version *) 0 ;
    Uint8 arg2 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(SDL_version **)&jarg1; 
    arg2 = (Uint8)jarg2; 
    if (arg1) (arg1)->minor = arg2;
    
}


JNIEXPORT jshort JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_get_1SDL_1version_1minor(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    SDL_version *arg1 = (SDL_version *) 0 ;
    Uint8 result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(SDL_version **)&jarg1; 
    result = (Uint8) ((arg1)->minor);
    
    jresult = (jshort)result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_set_1SDL_1version_1patch(JNIEnv *jenv, jclass jcls, jlong jarg1, jshort jarg2) {
    SDL_version *arg1 = (SDL_version *) 0 ;
    Uint8 arg2 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(SDL_version **)&jarg1; 
    arg2 = (Uint8)jarg2; 
    if (arg1) (arg1)->patch = arg2;
    
}


JNIEXPORT jshort JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_get_1SDL_1version_1patch(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jshort jresult = 0 ;
    SDL_version *arg1 = (SDL_version *) 0 ;
    Uint8 result;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(SDL_version **)&jarg1; 
    result = (Uint8) ((arg1)->patch);
    
    jresult = (jshort)result; 
    return jresult;
}


JNIEXPORT jlong JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_new_1SDL_1version(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    SDL_version *result;
    
    (void)jenv;
    (void)jcls;
    result = (SDL_version *)(SDL_version *) calloc(1, sizeof(SDL_version));
    
    *(SDL_version **)&jresult = result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_delete_1SDL_1version(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    SDL_version *arg1 = (SDL_version *) 0 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = *(SDL_version **)&jarg1; 
    free((char *) arg1);
    
}


JNIEXPORT jint JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_SDL_1Init(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    Uint32 arg1 ;
    int result;
    
    (void)jenv;
    (void)jcls;
    arg1 = (Uint32)jarg1; 
    result = (int)SDL_Init(arg1);
    
    jresult = (jint)result; 
    return jresult;
}


JNIEXPORT jint JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_SDL_1InitSubSystem(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jint jresult = 0 ;
    Uint32 arg1 ;
    int result;
    
    (void)jenv;
    (void)jcls;
    arg1 = (Uint32)jarg1; 
    result = (int)SDL_InitSubSystem(arg1);
    
    jresult = (jint)result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_SDL_1QuitSubSystem(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    Uint32 arg1 ;
    
    (void)jenv;
    (void)jcls;
    arg1 = (Uint32)jarg1; 
    SDL_QuitSubSystem(arg1);
    
}


JNIEXPORT jlong JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_SDL_1WasInit(JNIEnv *jenv, jclass jcls, jlong jarg1) {
    jlong jresult = 0 ;
    Uint32 arg1 ;
    Uint32 result;
    
    (void)jenv;
    (void)jcls;
    arg1 = (Uint32)jarg1; 
    result = (Uint32)SDL_WasInit(arg1);
    
    jresult = (jlong)result; 
    return jresult;
}


JNIEXPORT void JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_SDL_1Quit(JNIEnv *jenv, jclass jcls) {
    (void)jenv;
    (void)jcls;
    SDL_Quit();
    
}


JNIEXPORT jlong JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_SWIG_1SDL_1VERSION(JNIEnv *jenv, jclass jcls) {
    jlong jresult = 0 ;
    SDL_version result;
    
    (void)jenv;
    (void)jcls;
    result = SWIG_SDL_VERSION();
    
    {
        SDL_version * resultptr = (SDL_version *) malloc(sizeof(SDL_version));
        memmove(resultptr, &result, sizeof(SDL_version));
        *(SDL_version **)&jresult = resultptr;
    }
    return jresult;
}


JNIEXPORT jstring JNICALL Java_sdljava_x_swig_SWIG_1SDLMainJNI_SDL_1GetError(JNIEnv *jenv, jclass jcls) {
    jstring jresult = 0 ;
    char *result;
    
    (void)jenv;
    (void)jcls;
    result = (char *)SDL_GetError();
    
    {
        if(result) jresult = (*jenv)->NewStringUTF(jenv, result); 
    }
    return jresult;
}


#ifdef __cplusplus
}
#endif

