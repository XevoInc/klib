#ifndef KSON_H
#define KSON_H

#include <stdint.h>

#define KSON_TYPE_NO_QUOTE  1
#define KSON_TYPE_SGL_QUOTE 2
#define KSON_TYPE_DBL_QUOTE 3
#define KSON_TYPE_BRACKET   4
#define KSON_TYPE_BRACE     5

#define KSON_OK              0
#define KSON_ERR_EXTRA_LEFT  1
#define KSON_ERR_EXTRA_RIGHT 2
#define KSON_ERR_NO_KEY      3

typedef struct kson_node_s {
	uint64_t type:3, n:61;
	char *key;
	union {
		size_t *tmp; // a temporary pointer used by the parser; don't use this!!!
		const struct kson_node_s **child;
		char *str;
	} v;
} kson_node_t;

typedef struct {
	long n_nodes;
	kson_node_t *nodes; // nodes[0] is the root
} kson_t;

#define kson_is_internal(p) ((p)->type == KSON_TYPE_BRACKET || (p)->type == KSON_TYPE_BRACE)

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Parse a JSON string
	 *
	 * @param json    JSON string
	 * @param error   error code
	 *
	 * @return a pointer to kson_t if *error==0; or NULL otherwise
	 */
	kson_t *kson_parse(const char *json, int *error);

	/** Destroy a kson_t object */
	void kson_destroy(kson_t *kson);

	const kson_node_t *kson_query(const kson_node_t *root, int max_depth, ...);

	void kson_format(const kson_node_t *root);

#ifdef __cplusplus
}
#endif

static inline const kson_node_t *kson_by_key(const kson_node_t *p, const char *key)
{
	long i;
	for (i = 0; i < (long)p->n; ++i) {
		const kson_node_t *q = p->v.child[i];
		if (q->key && strcmp(q->key, key) == 0)
			return q;
	}
	return 0;
}

static inline const kson_node_t *kson_by_index(const kson_node_t *p, long i)
{
	return 0 <= i && i < (long)p->n? p->v.child[i] : 0;
}

#endif
