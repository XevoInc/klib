/*
Copyright 2018 Xevo Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

<http://www.apache.org/licenses/LICENSE-2.0>

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

---
xlib is originally derived from klib, which used the MIT license. The full text
of the klib license is:
---
Copyright (c) 2018 by Attractive Chaos <attractor@live.co.uk>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
---
*/

/*
  An example:

#include <xlib/xhash.h>
XHASH_MAP_INIT_INT(32, char)
int main() {
	int ret, is_missing;
	xhiter_t k;
	xhash_t(32) *h = xh_init(32);
	k = xh_put(32, h, 5, &ret);
	xh_value(h, k) = 10;
	k = xh_get(32, h, 10);
	is_missing = (k == xh_end(h));
	k = xh_get(32, h, 5);
	xh_del(32, h, k);
	for (k = xh_begin(h); k != xh_end(h); ++k)
		if (xh_exist(h, k)) xh_value(h, k) = 1;
	xh_destroy(32, h);
	return 0;
}
*/

#ifndef XLIB_XHASH_H_
#define XLIB_XHASH_H_

/*!
  @header

  Generic hash table library.
 */

#define AC_VERSION_XHASH_H "0.2.8"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

/* compiler specific configuration */

typedef uint_fast8_t xhint8_t;
typedef uint_fast16_t xhint16_t;
typedef uint_fast32_t xhint32_t;
typedef uint_fast64_t xhint64_t;

#ifndef xh_inline
#ifdef _MSC_VER
#define xh_inline __inline
#else
#define xh_inline inline
#endif
#endif /* xh_inline */

#ifndef klib_unused
#if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
#define klib_unused __attribute__ ((__unused__))
#else
#define klib_unused
#endif
#endif /* klib_unused */

typedef xhint32_t xhint_t;
typedef xhint_t xhiter_t;

#define __ac_isempty(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&2)
#define __ac_isdel(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&1)
#define __ac_iseither(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&3)
#define __ac_set_isdel_false(flag, i) (flag[i>>4]&=~(1ul<<((i&0xfU)<<1)))
#define __ac_set_isempty_false(flag, i) (flag[i>>4]&=~(2ul<<((i&0xfU)<<1)))
#define __ac_set_isboth_false(flag, i) (flag[i>>4]&=~(3ul<<((i&0xfU)<<1)))
#define __ac_set_isdel_true(flag, i) (flag[i>>4]|=1ul<<((i&0xfU)<<1))

#define __ac_fsize(m) ((m) < 16? 1 : (m)>>4)

#ifndef kroundup32
#define kroundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

#ifndef kcalloc
#define kcalloc(N,Z) calloc(N,Z)
#endif
#ifndef kmalloc
#define kmalloc(Z) malloc(Z)
#endif
#ifndef krealloc
#define krealloc(P,Z) realloc(P,Z)
#endif
#ifndef kfree
#define kfree(P) free(P)
#endif

static const double __ac_HASH_UPPER = 0.77;

#define __XHASH_TYPE(name, xhkey_t, xhval_t) \
	typedef struct xh_##name##_s { \
		xhint_t n_buckets, size, n_occupied, upper_bound; \
		xhint32_t *flags; \
		xhkey_t *keys; \
		xhval_t *vals; \
	} xh_##name##_t;

#define __XHASH_PROTOTYPES(name, xhkey_t, xhval_t)	 					\
	extern xh_##name##_t *xh_init_##name(void);							\
	extern void xh_destroy_##name(xh_##name##_t *h);					\
	extern void xh_clear_##name(xh_##name##_t *h);						\
	extern xhint_t xh_get_##name(const xh_##name##_t *h, xhkey_t key); 	\
	extern int xh_resize_##name(xh_##name##_t *h, xhint_t new_n_buckets); \
	extern xhint_t xh_put_##name(xh_##name##_t *h, xhkey_t key, int *ret); \
	extern void xh_del_##name(xh_##name##_t *h, xhint_t x);

#define __XHASH_IMPL(name, SCOPE, xhkey_t, xhval_t, xh_is_map, __hash_func, __hash_equal) \
	SCOPE xh_##name##_t *xh_init_##name(void) {							\
		return (xh_##name##_t*)kcalloc(1, sizeof(xh_##name##_t));		\
	}																	\
	SCOPE void xh_destroy_##name(xh_##name##_t *h)						\
	{																	\
		if (h) {														\
			kfree((void *)h->keys); kfree(h->flags);					\
			kfree((void *)h->vals);										\
			kfree(h);													\
		}																\
	}																	\
	SCOPE void xh_clear_##name(xh_##name##_t *h)						\
	{																	\
		if (h && h->flags) {											\
			memset(h->flags, 0xaa, __ac_fsize(h->n_buckets) * sizeof(xhint32_t)); \
			h->size = h->n_occupied = 0;								\
		}																\
	}																	\
	SCOPE xhint_t xh_get_##name(const xh_##name##_t *h, xhkey_t key) 	\
	{																	\
		if (h->n_buckets) {												\
			xhint_t k, i, last, mask, step = 0; \
			mask = h->n_buckets - 1;									\
			k = __hash_func(key); i = k & mask;							\
			last = i; \
			while (!__ac_isempty(h->flags, i) && (__ac_isdel(h->flags, i) || !__hash_equal(h->keys[i], key))) { \
				i = (i + (++step)) & mask; \
				if (i == last) return h->n_buckets;						\
			}															\
			return __ac_iseither(h->flags, i)? h->n_buckets : i;		\
		} else return 0;												\
	}																	\
	SCOPE int xh_resize_##name(xh_##name##_t *h, xhint_t new_n_buckets) \
	{ /* This function uses 0.25*n_buckets bytes of working space instead of [sizeof(key_t+val_t)+.25]*n_buckets. */ \
		xhint32_t *new_flags = 0;										\
		xhint_t j = 1;													\
		{																\
			kroundup32(new_n_buckets); 									\
			if (new_n_buckets < 4) new_n_buckets = 4;					\
			if (h->size >= (xhint_t)(new_n_buckets * __ac_HASH_UPPER + 0.5)) j = 0;	/* requested size is too small */ \
			else { /* hash table size to be changed (shrink or expand); rehash */ \
				new_flags = (xhint32_t*)kmalloc(__ac_fsize(new_n_buckets) * sizeof(xhint32_t));	\
				if (!new_flags) return -1;								\
				memset(new_flags, 0xaa, __ac_fsize(new_n_buckets) * sizeof(xhint32_t)); \
				if (h->n_buckets < new_n_buckets) {	/* expand */		\
					xhkey_t *new_keys = (xhkey_t*)krealloc((void *)h->keys, new_n_buckets * sizeof(xhkey_t)); \
					if (!new_keys) { kfree(new_flags); return -1; }		\
					h->keys = new_keys;									\
					if (xh_is_map) {									\
						xhval_t *new_vals = (xhval_t*)krealloc((void *)h->vals, new_n_buckets * sizeof(xhval_t)); \
						if (!new_vals) { kfree(new_flags); return -1; }	\
						h->vals = new_vals;								\
					}													\
				} /* otherwise shrink */								\
			}															\
		}																\
		if (j) { /* rehashing is needed */								\
			for (j = 0; j != h->n_buckets; ++j) {						\
				if (__ac_iseither(h->flags, j) == 0) {					\
					xhkey_t key = h->keys[j];							\
					xhval_t val;										\
					xhint_t new_mask;									\
					new_mask = new_n_buckets - 1; 						\
					if (xh_is_map) val = h->vals[j];					\
					__ac_set_isdel_true(h->flags, j);					\
					while (1) { /* kick-out process; sort of like in Cuckoo hashing */ \
						xhint_t k, i, step = 0; \
						k = __hash_func(key);							\
						i = k & new_mask;								\
						while (!__ac_isempty(new_flags, i)) i = (i + (++step)) & new_mask; \
						__ac_set_isempty_false(new_flags, i);			\
						if (i < h->n_buckets && __ac_iseither(h->flags, i) == 0) { /* kick out the existing element */ \
							{ xhkey_t tmp = h->keys[i]; h->keys[i] = key; key = tmp; } \
							if (xh_is_map) { xhval_t tmp = h->vals[i]; h->vals[i] = val; val = tmp; } \
							__ac_set_isdel_true(h->flags, i); /* mark it as deleted in the old hash table */ \
						} else { /* write the element and jump out of the loop */ \
							h->keys[i] = key;							\
							if (xh_is_map) h->vals[i] = val;			\
							break;										\
						}												\
					}													\
				}														\
			}															\
			if (h->n_buckets > new_n_buckets) { /* shrink the hash table */ \
				h->keys = (xhkey_t*)krealloc((void *)h->keys, new_n_buckets * sizeof(xhkey_t)); \
				if (xh_is_map) h->vals = (xhval_t*)krealloc((void *)h->vals, new_n_buckets * sizeof(xhval_t)); \
			}															\
			kfree(h->flags); /* free the working space */				\
			h->flags = new_flags;										\
			h->n_buckets = new_n_buckets;								\
			h->n_occupied = h->size;									\
			h->upper_bound = (xhint_t)(h->n_buckets * __ac_HASH_UPPER + 0.5); \
		}																\
		return 0;														\
	}																	\
	SCOPE xhint_t xh_put_##name(xh_##name##_t *h, xhkey_t key, int *ret) \
	{																	\
		xhint_t x;														\
		if (h->n_occupied >= h->upper_bound) { /* update the hash table */ \
			if (h->n_buckets > (h->size<<1)) {							\
				if (xh_resize_##name(h, h->n_buckets - 1) < 0) { /* clear "deleted" elements */ \
					*ret = -1; return h->n_buckets;						\
				}														\
			} else if (xh_resize_##name(h, h->n_buckets + 1) < 0) { /* expand the hash table */ \
				*ret = -1; return h->n_buckets;							\
			}															\
		} /* TODO: to implement automatically shrinking; resize() already support shrinking */ \
		{																\
			xhint_t k, i, site, last, mask = h->n_buckets - 1, step = 0; \
			x = site = h->n_buckets; k = __hash_func(key); i = k & mask; \
			if (__ac_isempty(h->flags, i)) x = i; /* for speed up */	\
			else {														\
				last = i; \
				while (!__ac_isempty(h->flags, i) && (__ac_isdel(h->flags, i) || !__hash_equal(h->keys[i], key))) { \
					if (__ac_isdel(h->flags, i)) site = i;				\
					i = (i + (++step)) & mask; \
					if (i == last) { x = site; break; }					\
				}														\
				if (x == h->n_buckets) {								\
					if (__ac_isempty(h->flags, i) && site != h->n_buckets) x = site; \
					else x = i;											\
				}														\
			}															\
		}																\
		if (__ac_isempty(h->flags, x)) { /* not present at all */		\
			h->keys[x] = key;											\
			__ac_set_isboth_false(h->flags, x);							\
			++h->size; ++h->n_occupied;									\
			*ret = 1;													\
		} else if (__ac_isdel(h->flags, x)) { /* deleted */				\
			h->keys[x] = key;											\
			__ac_set_isboth_false(h->flags, x);							\
			++h->size;													\
			*ret = 2;													\
		} else *ret = 0; /* Don't touch h->keys[x] if present and not deleted */ \
		return x;														\
	}																	\
	SCOPE void xh_del_##name(xh_##name##_t *h, xhint_t x)				\
	{																	\
		if (x != h->n_buckets && !__ac_iseither(h->flags, x)) {			\
			__ac_set_isdel_true(h->flags, x);							\
			--h->size;													\
		}																\
	}

#define XHASH_DECLARE(name, xhkey_t, xhval_t)		 					\
	__XHASH_TYPE(name, xhkey_t, xhval_t) 								\
	__XHASH_PROTOTYPES(name, xhkey_t, xhval_t)

#define XHASH_INIT2(name, SCOPE, xhkey_t, xhval_t, xh_is_map, __hash_func, __hash_equal) \
	__XHASH_TYPE(name, xhkey_t, xhval_t) 								\
	__XHASH_IMPL(name, SCOPE, xhkey_t, xhval_t, xh_is_map, __hash_func, __hash_equal)

#define XHASH_INIT(name, xhkey_t, xhval_t, xh_is_map, __hash_func, __hash_equal) \
	XHASH_INIT2(name, static xh_inline klib_unused, xhkey_t, xhval_t, xh_is_map, __hash_func, __hash_equal)

/* --- BEGIN OF HASH FUNCTIONS --- */

/*! @function
  @abstract     Integer hash function
  @param  key   The integer [xhint32_t]
  @return       The hash value [xhint_t]
 */
#define xh_int_hash_func(key) (xhint32_t)(key)
/*! @function
  @abstract     Integer comparison function
 */
#define xh_int_hash_equal(a, b) ((a) == (b))
/*! @function
  @abstract     Integer hash function
  @param  key   The integer [xhint8_t]
  @return       The hash value [xhint_t]
 */
#define xh_int8_hash_func(key) (xhint_t)(key)
/*! @function
  @abstract     Integer comparison function
 */
#define xh_int8_hash_equal(a, b) ((a) == (b))
/*! @function
  @abstract     Integer hash function
  @param  key   The integer [xhint16_t]
  @return       The hash value [xhint_t]
 */
#define xh_int16_hash_func(key) (xhint_t)(key)
/*! @function
  @abstract     Integer comparison function
 */
#define xh_int16_hash_equal(a, b) ((a) == (b))
/*! @function
  @abstract     64-bit integer hash function
  @param  key   The integer [xhint64_t]
  @return       The hash value [xhint_t]
 */
#define xh_int64_hash_func(key) (xhint32_t)((key)>>33^(key)^(key)<<11)

#if UINTPTR_MAX <= UINT_MAX /* 32-bit or less */
#define xh_ptr_hash_equal(a, b) xh_int_hash_equal((xhint32_t) (a), (xhint32_t) (b))
#define xh_ptr_hash_func(key) xh_int_hash_func((xhint32_t) (key))
#else /* 64-bit */
#define xh_ptr_hash_equal(a, b) xh_int64_hash_equal((xhint64_t) (a), (xhint64_t) (b))
#define xh_ptr_hash_func(key) xh_int64_hash_func((xhint64_t) (key))
#endif

/*! @function
  @abstract     64-bit integer comparison function
 */
#define xh_int64_hash_equal(a, b) ((a) == (b))
/*! @function
  @abstract     const char* hash function
  @param  s     Pointer to a null terminated string
  @return       The hash value
 */
static xh_inline xhint_t __ac_X31_hash_string(const char *s)
{
	xhint_t h = (xhint_t)*s;
	if (h) for (++s ; *s; ++s) h = (h << 5) - h + (xhint_t)*s;
	return h;
}
/*! @function
  @abstract     Another interface to const char* hash function
  @param  key   Pointer to a null terminated string [const char*]
  @return       The hash value [xhint_t]
 */
#define xh_str_hash_func(key) __ac_X31_hash_string(key)
/*! @function
  @abstract     Const char* comparison function
 */
#define xh_str_hash_equal(a, b) (strcmp(a, b) == 0)

static xh_inline xhint_t __ac_Wang_hash(xhint_t key)
{
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}
#define xh_int_hash_func2(key) __ac_Wang_hash((xhint_t)key)

/* --- END OF HASH FUNCTIONS --- */

/* Other convenient macros... */

/*!
  @abstract Type of the hash table.
  @param  name  Name of the hash table [symbol]
 */
#define xhash_t(name) xh_##name##_t

/*! @function
  @abstract     Initiate a hash table.
  @param  name  Name of the hash table [symbol]
  @return       Pointer to the hash table [xhash_t(name)*]
 */
#define xh_init(name) xh_init_##name()

/*! @function
  @abstract     Destroy a hash table.
  @param  name  Name of the hash table [symbol]
  @param  h     Pointer to the hash table [xhash_t(name)*]
 */
#define xh_destroy(name, h) xh_destroy_##name(h)

/*! @function
  @abstract     Reset a hash table without deallocating memory.
  @param  name  Name of the hash table [symbol]
  @param  h     Pointer to the hash table [xhash_t(name)*]
 */
#define xh_clear(name, h) xh_clear_##name(h)

/*! @function
  @abstract     Resize a hash table.
  @param  name  Name of the hash table [symbol]
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  s     New size [xhint_t]
 */
#define xh_resize(name, h, s) xh_resize_##name(h, s)


/*! @function
  @abstract     Resize the hash table so that current size == max size.
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  s     New size [xhint_t]
 */
#define xh_trim(name, h) (xh_resize(name, h, xh_size(h)))

/*! @function
  @abstract     Insert a key to the hash table.
  @param  name  Name of the hash table [symbol]
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  k     Key [type of keys]
  @param  r     Extra return code: -1 if the operation failed;
                0 if the key is present in the hash table;
                1 if the bucket is empty (never used); 2 if the element in
				the bucket has been deleted [int*]
  @return       Iterator to the inserted element [xhint_t]
 */
#define xh_put(name, h, k, r) xh_put_##name(h, k, r)

/*! @function
  @abstract     Retrieve a key from the hash table.
  @param  name  Name of the hash table [symbol]
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  k     Key [type of keys]
  @return       Iterator to the found element, or xh_end(h) if the element is absent [xhint_t]
 */
#define xh_get(name, h, k) xh_get_##name(h, k)

/*! @function
  @abstract     Remove a key from the hash table.
  @param  name  Name of the hash table [symbol]
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  k     Iterator to the element to be deleted [xhint_t]
 */
#define xh_del(name, h, k) xh_del_##name(h, k)

/*! @function
  @abstract     Test whether a bucket contains data.
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  x     Iterator to the bucket [xhint_t]
  @return       1 if containing data; 0 otherwise [int]
 */
#define xh_exist(h, x) (!__ac_iseither((h)->flags, (x)))

/*! @function
  @abstract     Get key given an iterator
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  x     Iterator to the bucket [xhint_t]
  @return       Key [type of keys]
 */
#define xh_key(h, x) ((h)->keys[x])

/*! @function
  @abstract     Get value given an iterator
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  x     Iterator to the bucket [xhint_t]
  @return       Value [type of values]
  @discussion   For hash sets, calling this results in segfault.
 */
#define xh_val(h, x) ((h)->vals[x])

/*! @function
  @abstract     Alias of xh_val()
 */
#define xh_value(h, x) ((h)->vals[x])

/*! @function
  @abstract     Get the start iterator
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @return       The start iterator [xhint_t]
 */
#define xh_begin(h) (xhint_t)(0)

/*! @function
  @abstract     Get the end iterator
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @return       The end iterator [xhint_t]
 */
#define xh_end(h) ((h)->n_buckets)

/*! @function
  @abstract     Get the number of elements in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @return       Number of elements in the hash table [xhint_t]
 */
#define xh_size(h) ((h)->size)

/*! @function
  @abstract     Get the number of buckets in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @return       Number of buckets in the hash table [xhint_t]
 */
#define xh_n_buckets(h) ((h)->n_buckets)

/*! @function
  @abstract     Check if a key exists in the map.
  @param  name  Name of the hash table [symbol]
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  k     Key [type of keys]
  @return       true if the key is found in the map, false otherwise
 */
#define xh_found(name, h, k) (xh_get_##name(h, k) != xh_end(h))

/*! @function
  @abstract     Iterate over the entries in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  ivar  The iterator variable
  @param  code  Block of code to execute
 */
#define xh_iter(h, ivar, code) {									\
	for ((ivar) = xh_begin(h); (ivar) != xh_end(h); ++(ivar)) {		\
		if (!xh_exist(h,ivar)) continue;							\
		code;														\
	} }

/*! @function
  @abstract     Iterate over the entries in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  kvar  Variable to which key will be assigned
  @param  vvar  Variable to which value will be assigned
  @param  code  Block of code to execute
 */
#define xh_foreach(h, kvar, vvar, code) { xhint_t __i;		\
	xh_iter(h, __i,											\
		(kvar) = xh_key(h,__i);								\
		(vvar) = xh_val(h,__i);								\
		code;												\
	) }														\

/*! @function
  @abstract     Iterate over the entries in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  kvar  Variable to which key will be assigned
  @param  vvar  Variable to which value will be assigned
  @param  ivar  The iterator variable
  @param  code  Block of code to execute
 */
#define xh_foreach_iter(h, kvar, vvar, ivar, code) {		\
	xh_iter(h, ivar,										\
		(kvar) = xh_key(h,__i);								\
		(vvar) = xh_val(h,__i);								\
		code;												\
	) }														\

/*! @function
  @abstract     Iterate over the keys in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  kvar  Variable to which key will be assigned
  @param  code  Block of code to execute
 */
#define xh_foreach_key(h, kvar, code) {	xhint_t __i;		\
	xh_iter(h, __i,											\
		(kvar) = xh_key(h,__i);								\
		code;												\
	) }														\

/*! @function
  @abstract     Iterate over the keys in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  kvar  Variable to which key will be assigned
  @param  ivar  The iterator variable
  @param  code  Block of code to execute
 */
#define xh_foreach_key_iter(h, kvar, ivar, code) {			\
	xh_iter(h, ivar,										\
		(kvar) = xh_key(h,ivar);							\
		code;												\
	) }														\

/*! @function
  @abstract     Iterate over the values in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  vvar  Variable to which value will be assigned
  @param  code  Block of code to execute
 */
#define xh_foreach_value(h, vvar, code) { xhint_t __i;		\
	xh_iter(h, __i,											\
		(vvar) = xh_val(h,__i);								\
		code;												\
	) }														\

/*! @function
  @abstract     Iterate over the values in the hash table
  @param  h     Pointer to the hash table [xhash_t(name)*]
  @param  vvar  Variable to which value will be assigned
  @param  ivar  The iterator variable
  @param  code  Block of code to execute
 */
#define xh_foreach_value_iter(h, vvar, ivar, code) {		\
	xh_iter(h, ivar,										\
		(vvar) = xh_val(h,ivar);							\
		code;												\
	) }														\

/* More conenient interfaces */

/*! @function
  @abstract     Instantiate a hash set containing integer keys
  @param  name  Name of the hash table [symbol]
 */
#define XHASH_SET_INIT_INT(name)										\
	XHASH_INIT(name, xhint32_t, char, 0, xh_int_hash_func, xh_int_hash_equal)

/*! @function
  @abstract     Instantiate a hash map containing integer keys
  @param  name  Name of the hash table [symbol]
  @param  xhval_t  Type of values [type]
 */
#define XHASH_MAP_INIT_INT(name, xhval_t)								\
	XHASH_INIT(name, xhint32_t, xhval_t, 1, xh_int_hash_func, xh_int_hash_equal)

/*! @function
  @abstract     Instantiate a hash set containing integer keys
  @param  name  Name of the hash table [symbol]
 */
#define XHASH_SET_INIT_INT8(name)										\
	XHASH_INIT(name, xhint8_t, char, 0, xh_int8_hash_func, xh_int8_hash_equal)

/*! @function
  @abstract     Instantiate a hash map containing integer keys
  @param  name  Name of the hash table [symbol]
  @param  xhval_t  Type of values [type]
 */
#define XHASH_MAP_INIT_INT8(name, xhval_t)								\
	XHASH_INIT(name, xhint8_t, xhval_t, 1, xh_int8_hash_func, xh_int8_hash_equal)

/*! @function
  @abstract     Instantiate a hash set containing integer keys
  @param  name  Name of the hash table [symbol]
 */

#define XHASH_SET_INIT_INT16(name)										\
	XHASH_INIT(name, xhint16_t, char, 0, xh_int16_hash_func, xh_int16_hash_equal)

/*! @function
  @abstract     Instantiate a hash map containing integer keys
  @param  name  Name of the hash table [symbol]
  @param  xhval_t  Type of values [type]
 */
#define XHASH_MAP_INIT_INT16(name, xhval_t)								\
	XHASH_INIT(name, xhint16_t, xhval_t, 1, xh_int16_hash_func, xh_int16_hash_equal)

/*! @function
  @abstract     Instantiate a hash map containing 64-bit integer keys
  @param  name  Name of the hash table [symbol]
 */
#define XHASH_SET_INIT_INT64(name)										\
	XHASH_INIT(name, xhint64_t, char, 0, xh_int64_hash_func, xh_int64_hash_equal)

/*! @function
  @abstract     Instantiate a hash map containing 64-bit integer keys
  @param  name  Name of the hash table [symbol]
  @param  xhval_t  Type of values [type]
 */
#define XHASH_MAP_INIT_INT64(name, xhval_t)								\
	XHASH_INIT(name, xhint64_t, xhval_t, 1, xh_int64_hash_func, xh_int64_hash_equal) /* NOLINT */

/*! @function
  @abstract     Instantiate a hash map containing pointer keys
  @param  ptr_type A pointer type [type]
  @param  name  Name of the hash table [symbol]
 */
#define XHASH_SET_INIT_PTR(name, ptr_type)								\
	XHASH_INIT(name, ptr_type, char, 0, xh_ptr_hash_func, xh_ptr_hash_equal)

/*! @function
  @abstract     Instantiate a hash map containing pointer keys
  @param  name  Name of the hash table [symbol]
  @param  ptr_type A pointer type [type]
  @param  xhval_t  Type of values [type]
 */
#define XHASH_MAP_INIT_PTR(name, ptr_type, xhval_t)						\
	XHASH_INIT(name, ptr_type, xhval_t, 1, xh_ptr_hash_func, xh_ptr_hash_equal) /* NOLINT */

typedef const char *xh_cstr_t;
/*! @function
  @abstract     Instantiate a hash map containing const char* keys
  @param  name  Name of the hash table [symbol]
 */
#define XHASH_SET_INIT_STR(name)										\
	XHASH_INIT(name, xh_cstr_t, char, 0, xh_str_hash_func, xh_str_hash_equal)

/*! @function
  @abstract     Instantiate a hash map containing const char* keys
  @param  name  Name of the hash table [symbol]
  @param  xhval_t  Type of values [type]
 */
#define XHASH_MAP_INIT_STR(name, xhval_t)								\
	XHASH_INIT(name, xh_cstr_t, xhval_t, 1, xh_str_hash_func, xh_str_hash_equal)

#endif /* XLIB_XHASH_H_ */
