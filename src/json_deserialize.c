#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct {
	char *buf;
	size_t len;
} json_string_context;

typedef struct {
	char *src;
	size_t len;
	size_t i;
} json_deserialize_state;

char current(json_deserialize_state *s) {
	if (s->i > s->len) {
		return -1;
	}
	return s->src[s->i];
}

#define lookahead(s, cur) (current((s)) == (cur))

void advance(json_deserialize_state *s) {
	s->i++;
}

void retreat(json_deserialize_state *s) {
	s->i--;
}

int accept(json_deserialize_state *s, char c) {
	if (s->i > s->len) {
		return 0;
	}
	if (current(s) != c) {
		return 0;
	}
	advance(s);
	return 1;
}

int json_null(json_deserialize_state *s) {
	if (
		   accept(s, 'n') 
		&& accept(s, 'u') 
		&& accept(s, 'l') 
		&& accept(s, 'l')
	) {
		return s->i - 3;
	}

	return 0;
}

int json_true(json_deserialize_state *s) {
	if (
			accept(s, 't') 
		 && accept(s, 'r') 
		 && accept(s, 'u') 
		 && accept(s, 'e')
	 ) {
		return s->i - 3;
	}

	return 0;
}

int json_false(json_deserialize_state *s) {
	if (
			accept(s, 'f') 
		 && accept(s, 'a') 
		 && accept(s, 'l') 
		 && accept(s, 's') 
		 && accept(s, 'e')
	) {
		return s->i - 4;
	}

	return 0;
}

int json_string(json_deserialize_state *s) {
	/* vec(char) str;
	 vec_make(&str);*/
	if (accept(s, '"')) {
		while (s->i < s->len) {
			if (accept(s, '"')) {
				return 1;
			} else if (accept(s, '\\')) {
				if (accept(s, '\\')) {
					continue;
				} if (accept(s, '/')) {
					continue;
				} if (accept(s, 'b')) {
					continue;
				} if (accept(s, 'f')) {
					continue;
				} if (accept(s, 'n')) {
					continue;
				} if (accept(s, 'r')) {
					continue;
				} if (accept(s, 't')) {
					continue;
				} if (accept(s, 'u')) {
					// todo
					continue;
				}
			}
		}
	}
	return 0;
}

int json_object(json_deserialize_state *s) {
	if (accept(s, '{')) {

	}
	return 0;
}


int json_array(json_deserialize_state *s) {

	return 0;
}

int json_whitespace(json_deserialize_state *s) {
	return accept(s, '\t') 
		|| accept(s, '\n') 
		|| accept(s, '\r') 
		|| accept(s, ' ');
}

int json_value(json_deserialize_state *s) {
	if (lookahead(s, 'n')) {
		return json_null(s);
	} else if (lookahead(s, 't')) {
		return json_true(s);
	} else if (lookahead(s, 'f')) {
		return json_false(s);
	} else if (lookahead(s, '"')) {
		return json_string(s);
	} else if (lookahead(s, '{')) {
		return json_object(s);
	} else if (lookahead(s, '[')) {
		return json_array(s);
	} else if (json_whitespace(s)) { 
		return json_value(s); 
	}
	return 0;
}
