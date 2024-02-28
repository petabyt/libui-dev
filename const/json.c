#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

#define JSON_OBJ '{'
#define JSON_ARR '['
#define JSON_STR 's'
#define JSON_NUM 'n'

#define JSON_END_OBJ '}'
#define JSON_END_ARR ']'
#define JSON_COMMA ','
#define JSON_KEY ':'

static int next(struct Parse *p) {
	char *b = p->b;

	while (*b == ' ' || *b == '\n' || *b == '\t') {
		b++;
	}

	if (*b == '\0') {
		return 1;
	}

	int type;

	if (*b == '{') {
		type = JSON_OBJ;
	} else if (*b == '[') {
		type = JSON_ARR;
	} else if (*b == '\"' || *b == '\'') {
		char quote_type = *b;
		type = JSON_STR;
		b++;
		p->str = b;
		p->str_len = 0;
		while (*b != quote_type) {
			if (*b == '\\') {
				b++;
				p->str_len++;
			}
			b++;
			p->str_len++;
		}
	} else if (*b >= '0' && *b <= '9') {
		type = JSON_NUM;
		unsigned int v = 0;
		while (*b >= '0' && *b <= '9') {
			v *= 10;
			v += (*b) - '0';
			b++;
		}
		p->value = v;
		b--;
	} else if (*b == ':') {
		type = JSON_KEY;
	} else if (*b == '}') {
		type = JSON_END_OBJ;
	} else if (*b == ']') {
		type = JSON_END_ARR;
	} else if (*b == ',') {
		type = JSON_COMMA;
	} else {
		printf("Unknown tok %c\n", *b);
		return 1;
	}

	p->type = type;

	b++;
	p->b = b;

	return 0;
}

static int next_intent(struct Parse *p) {
	struct Parse p2;
	p2.b = p->intent;

	if (next(&p2)) return 1;

	if (p2.type == JSON_ARR) {
		next(&p2);
		if (p2.type == JSON_STR) {
			strncpy(p->intent_key, p2.str, p2.str_len);
			p->intent_type = JSON_STR;
		} else if (p2.type == JSON_NUM) {
			p->intent_i = (int)p2.value;
			p->intent_type = JSON_NUM;
		}
	}

	if (next(&p2)) return 1;

	p->intent = p2.b;

	return 0;
}

static int parse_block(struct Parse *p, int skip);

static int parse_array(struct Parse *p, int skip) {
	int i = 0;
	while (!next(p)) {
		int ignore = 1;
		if (!skip) {
			if (p->intent_i == i) {
				next_intent(p);
				ignore = 0;
			}
		}

		if (p->type == JSON_STR) {
			if (!ignore) return 1;
		} else if (p->type == JSON_NUM) {
			if (!ignore) return 1;
		} else if (p->type == JSON_OBJ) {
			parse_block(p, ignore);
			if (!ignore) return 1;
		} else if (p->type == JSON_ARR) {
			parse_array(p, ignore);
			if (!ignore) return 1;
		} else if (p->type == JSON_END_ARR) {
			if (!skip && (p->intent_i > i)) {
				p->out_of_bounds = 1;
				return 1;
			}
			return 0;
		} else if (p->type == JSON_COMMA) {
			i++;
			continue;
		} else {
			printf("arr: unexpected type %c\n", p->type);
		}
	}

	return 1;
}

static int parse_block(struct Parse *p, int skip) {
	while (!next(p)) {
		if (p->type == JSON_STR) {
			int ignore = 1; char *prev = 0;
			if (!skip && p->intent_type == JSON_STR) {
				if (!strncmp(p->str, p->intent_key, p->str_len)) {
					ignore = 0;
					if (next_intent(p)) {
						next(p); // skip JSON_KEY
						prev = p->b; // save token pos
					}
				}
			}
			if (!prev) next(p);
			if (p->type == JSON_KEY) {
				next(p); // we need to know if this is a dead end or not
				if (prev) {
					if (p->type == JSON_STR || p->type == JSON_NUM) return 1; // we are at str/int, no more children to parse
					if (p->type == JSON_OBJ || p->type == JSON_ARR) {  p->b = prev; return 1; } // we have children to parse, go back
				}
				if (p->type == JSON_OBJ) {
					if (parse_block(p, ignore)) return 1;
				} else if (p->type == JSON_ARR) {
					if (parse_array(p, ignore)) return 1;
				}

				if (!ignore) return 1;
			}
			next(p);
			if (p->type == JSON_END_OBJ) {
				return 0;
			} else if (p->type != JSON_COMMA) {
				printf("Expected comma, %c\n", p->type);
			}
		} else if (p->type == JSON_END_OBJ) {
			return 0;
		} else {
			printf("key: expected type %c\n", p->type);
		}
	}

	return 1;
}

int json_get(struct Parse *p, char *text, char *intent) {
	p->intent = intent;	
	p->b = text;
	next_intent(p);

	next(p);
	if (p->type == JSON_OBJ) {
		parse_block(p, 0);
	} else if (p->type == JSON_ARR) {
		parse_array(p, 0);
	} else {
		printf("Unsupported type to start at %c\n", p->type);
	}

	return 0;
}

// buf size must be at least p->str_len + 1
char *json_parse_string(struct Parse *p, char *buf) {
	int c2 = 0;
	for (int c = 0; c < p->str_len; c++) {
		if (p->str[c] == '\\') {
			if (p->str[c + 1] == 'n')
				buf[c2] = '\n';
			if (p->str[c + 1] == '"')				
				buf[c2] = '"';
			c++;
		} else {
			buf[c2] = p->str[c];
		}
		c2++;
		if (c2 > p->str_len) {
			return "Out of bounds bug";
		}
	}
	buf[c2] = '\0';
	return buf;
}

// Optional
char *json_fixup_string(struct Parse *p) {
	char *buf = malloc(p->str_len + 1);
	return json_parse_string(p, buf);
}
