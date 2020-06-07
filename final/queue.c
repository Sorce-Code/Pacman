/* queue.c ---
 * 
 *   Author: Roy Sorce 
 *   Partner: Sarah Sha
 *   Date Created: 4/1/18 
 *   Last Modified by: Roy Sorce 
 *   Date Last Modified: 4/5/18
 *   Assignment: Lab10
 *   Part of: C335
 */ 

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 * 
 * 
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

#include "queue.h"

void init_queue(queue_t *buf) { 
  buf->tail = 0;
  buf->head = 0; //unsure about these initializations...
}

int enqueue (queue_t *buf, int data) {
  if(queue_full(buf)){
    return 0;
  } else {
    buf->buffer[buf->head] = data;
    buf->head = ((buf->head + 1) == QUEUE_SIZE) ? 0 : buf->head + 1;
  }
  return 1;
}

int dequeue (queue_t *buf, int *data) {
  if(queue_empty(buf)){
    return 0;
  } else{
    *data = buf->buffer[buf->tail];
    buf->tail = ((buf->tail + 1) ==QUEUE_SIZE) ? 0 : buf->tail + 1;
  }
  return 1;
}

int queue_empty(queue_t *buf) {
  return (buf->head == buf->tail);
}

int queue_full(queue_t *buf) {
  return (((buf->head + 1) % QUEUE_SIZE) == buf->tail);
}


/* queue.c ends here */
