struct Parse {
	char *b; // current lexer ptr
	char *intent; // current lexer ptr for intent
	char intent_type;
	char intent_key[16]; // key value for intent
	int intent_i; // index for intent
	char out_of_bounds; // will be set to 1 if out of bounds in array access
	char type;
	char *str;
	int str_len;
	unsigned int value;
};

int json_get(struct Parse *p, char *text, char *intent);
char *json_fixup_string(struct Parse *p);
