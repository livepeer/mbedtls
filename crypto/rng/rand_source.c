/*
 * rand_source.c
 *
 * implements a random source based on /dev/random
 *
 * David A. McGrew
 * Cisco Systems, Inc.
 */
/*
 *	
 * Copyright(c) 2001-2005 Cisco Systems, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 * 
 *   Neither the name of the Cisco Systems, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"
#include "rand_source.h"

#ifdef DEV_URANDOM
#include <fcntl.h>          /* for open()  */
#include <unistd.h>         /* for close() */
#endif

/* global dev_rand_fdes is file descriptor for /dev/random */

int dev_random_fdes = 0;


err_status_t
rand_source_init() {

#ifdef DEV_URANDOM
  /* open random source for reading */
  dev_random_fdes = open(DEV_URANDOM, O_RDONLY, 0);
  if (dev_random_fdes == 0)
    return err_status_init_fail;
  return err_status_ok;
#else
  /* no random source available; let the caller know */
  return err_status_fail;
#endif
}

err_status_t
rand_source_get_octet_string(void *dest, uint32_t len) {

  /* 
   * read len octets from /dev/random to dest, and
   * check return value to make sure enough octets were
   * written 
   */
#ifdef DEV_URANDOM
  if (read(dev_random_fdes, dest, len) != len)
    return err_status_fail;
#else
  /* Generic C-library (rand()) version */
  /* This is a random source of last resort */
  uint8_t *dst = dest;
  while (len)
  {
	  int val = rand();
	  /* rand() returns 0-32767 (ugh) */
	  /* Is this a good enough way to get random bytes?
	     It is if it passes FIPS-140... */
	  *dst++ = val & 0xff;
	  len--;
  }
#endif
  return err_status_ok;
}

err_status_t
rand_source_deinit() {

#ifdef DEV_URANDOM
  if (dev_random_fdes == 0)
    return err_status_dealloc_fail;  /* well, we haven't really failed, *
				      * but there is something wrong    */
  close(dev_random_fdes);  
#endif
  
  return err_status_ok;  
}
