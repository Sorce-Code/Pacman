/* queue.h ---
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

#define QUEUE_SIZE 32

typedef struct queue {
  int head;
  int tail;
  int buffer[QUEUE_SIZE];
} queue_t;

void init_queue(queue_t *);
int enqueue(queue_t *, int);
int dequeue(queue_t *, int *);            
int queue_empty(queue_t *);            
int queue_full(queue_t *);

/* queue.h ends here */
