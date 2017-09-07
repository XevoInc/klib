/*
 * This is an optimized version of the following C++ program:
 *
 *   http://keithlea.com/javabench/src/cpp/hash.cpp
 *
 * Keith in his benchmark (http://keithlea.com/javabench/data) showed that the
 * Java implementation is twice as fast as the C++ version. In fact, this is
 * only because the C++ implementation is substandard. Most importantly, Keith
 * is using "sprintf()" to convert an integer to a string, which is known to be
 * extremely inefficient.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <xlib/xhash.h>
XHASH_MAP_INIT_STR(str, int)

static inline void int2str(int c, int base, char *ret)
{
	const char *tab = "0123456789abcdef";
	if (c == 0) ret[0] = '0', ret[1] = 0;
	else {
		int l, x, y;
		char buf[16];
		for (l = 0, x = c < 0? -c : c; x > 0; x /= base) buf[l++] = tab[x%base];
		if (c < 0) buf[l++] = '-';
		for (x = l - 1, y = 0; x >= 0; --x) ret[y++] = buf[x];
		ret[y] = 0;
	}
}

int main(int argc, char *argv[])
{
	int i, n = 1000, ret;
	xhash_t(str) *h, *h2;
	xhint_t k;
	h = xh_init(str);
	h2 = xh_init(str);
	if (argc > 1) n = atoi(argv[1]);
	for (i = 0; i < 10000; ++i) {
		char buf[32];
		strcpy(buf, "foo_");
		int2str(i, 10, buf+4);
		k = xh_put(str, h, strdup(buf), &ret);
		xh_val(h, k) = i;
	}
	for (i = 0; i < n; ++i) {
		for (k = xh_begin(h); k != xh_end(h); ++k) {
			if (xh_exist(h, k)) {
				xhint_t k2 = xh_put(str, h2, xh_key(h, k), &ret);
				if (ret) { // absent
					xh_key(h2, k2) = strdup(xh_key(h, k));
					xh_val(h2, k2) = xh_val(h, k);
				} else xh_val(h2, k2) += xh_val(h, k);
			}
		}
	}
	k = xh_get(str, h, "foo_1"); printf("%d", xh_val(h, k));
	k = xh_get(str, h, "foo_9999"); printf(" %d", xh_val(h, k));
	k = xh_get(str, h2, "foo_1"); printf(" %d", xh_val(h2, k));
	k = xh_get(str, h2, "foo_9999"); printf(" %d\n", xh_val(h2, k));
	for (k = xh_begin(h); k != xh_end(h); ++k)
		if (xh_exist(h, k)) free((char*)xh_key(h, k));
	for (k = xh_begin(h2); k != xh_end(h2); ++k)
		if (xh_exist(h2, k)) free((char*)xh_key(h2, k));
	xh_destroy(str, h);
	xh_destroy(str, h2);
	return 0;
}
