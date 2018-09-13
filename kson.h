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

typedef struct {
	uint64_t type:3, n:61;
	char *key;
	union {
		int *child;
		char *str;
	} v;
} kson_node_t;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Parse a JSON string
	 *
	 * @param json        JSON string
	 * @param n_nodes     number of nodes
	 * @param error       0 if no error; or set to one of KSON_ERR_* values
	 * @param parsed_len  if not NULL, equal to the parsed length
	 *
	 * @return An array of size $n_nodes keeping parsed nodes
	 */
	kson_node_t *kson_parse(const char *json, int *n_nodes, int *error, int *parsed_len);

	void kson_destroy(int n_nodes, kson_node_t *nodes);

	void kson_print_recur(kson_node_t *nodes, kson_node_t *root);

#ifdef __cplusplus
}
#endif

#endif
