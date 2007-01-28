/***************************************************************************
                           rlthread.h  -  description
                              -------------------
     begin                : Tue Jan 02 2001
     copyright            : (C) 2001 by Rainer Lehrig
     email                : lehrig@t-online.de
  ***************************************************************************/
 
 /***************************************************************************
  *                                                                         *
  *   This library is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
  *   published by the Free Software Foundation                             *
  *                                                                         *
  ***************************************************************************/
 #ifndef _RL_THREAD_H_
 #define _RL_THREAD_H_
 
 #include "rldefine.h"
 #include "rlwthread.h"
 
 class rlThread;
 
 typedef struct
 {
   rlThread *thread;
   void     *user;
   int      running;
 }
 THREAD_PARAM;
 
 class rlThread
 {
 public:
   rlThread(int max_semphore=1000);
   ~rlThread();
   int create(void *(*func)(void*), void *argument);
 
   int trylock();
 
   int lock();
 
   int unlock();
 
   int waitSemaphore();
 
   int incrementSemaphore();
 
   int join(void **status);
 
   int cancel();
 
   void threadExit(void *status);
 
   pthread_t       tid;
   pthread_attr_t  attr;
   pthread_mutex_t mutex;
   WSEMAPHORE      semaphore;
 private:
   THREAD_PARAM    arg;
 };
 
 class rlMutex
 {
 public:
   rlMutex(const pthread_mutexattr_t *attr = NULL);
   ~rlMutex();
 
   int trylock();
 
   int lock();
 
   int unlock();
 
   pthread_mutex_t mutex;
 };
 
 class rlSemaphore
 {
 public:
   rlSemaphore(int max_semaphore = 1000);
   ~rlSemaphore();
 
   int waitSemaphore();
 
   int incrementSemaphore();
 
   WSEMAPHORE      semaphore;
 };
 
 #endif
