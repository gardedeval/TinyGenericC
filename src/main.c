#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector.h>
#include <linked_list.h>
#include <math.h>

#include "hash_table.h"
#include "json.h"
#include "ref_count.h"

void test_vector() {
    size_t i;
    vec(float) v;
    vec_make(&v);

    for (i = 0; i <= 1024; i++) {
        vec_at_expand(&v, i) = (float)sqrt(i);
    }

    for (i = 0; i <= 1024; i++) {
        common_ensure(vec_at(&v, i) == (float)sqrt(i));
    }

    {
        vec(float) v2;
        size_t i;
        float *value;

        vec_clone(&v2, &v);

        vec_for_each(&v2, i, value) {
            common_ensure(*value == vec_at(&v, i));
        }

        vec_destroy(&v2);
    }

    {
        common_ensure(vec_pop(&v) == 32);

        vec_enqueue(&v, 4);
        vec_enqueue(&v, 3);
        vec_enqueue(&v, 2);
        vec_enqueue(&v, 1);

        common_ensure(vec_bottom(&v) == 1);
        common_ensure(vec_get(&v, 1) == 2);
        common_ensure(vec_get(&v, 2) == 3);
        common_ensure(vec_get(&v, 3) == 4);

        common_ensure(vec_dequeue(&v) == 1);
        common_ensure(vec_dequeue(&v) == 2);
        common_ensure(vec_dequeue(&v) == 3);
        common_ensure(vec_dequeue(&v) == 4);
    }

    vec_destroy(&v);
}

void test_linked_list() {
    typedef ll_t(int) int_ll;
    
    int_ll one;
    int_ll two;
    int_ll three;
    int_ll four;
    int_ll five;
    int_ll six;

    int i = 1;
    ll_make(&one) = i++;
    ll_make(&two) = i++;
    ll_make(&three) = i++;
    ll_make(&four) = i++;
    ll_make(&five) = i++;
    ll_make(&six) = i++;

    /* 1 -> 2 */
    ll_mergel(&one, &two);
    /* (1 -> 2) <- 3 */
    ll_merger(&two, &three);
    /* (1 -> 2 -> 3) -> 4 */
    ll_mergel(&three, &four);
    /* 6 -> (1 -> 2 -> 3 -> 4) */
    ll_mergel(&six, &one);
    /* (6 -> 1 -> 2) <- 5 -> 3 -> 4 */
    ll_merger(&two, &five);

    int_ll *it;
    it = &six;
    common_ensure(it->value == 6);
    it = it->next;
    common_ensure(it->value == 1);
    it = it->next;
    common_ensure(it->value == 2);
    it = it->next;
    common_ensure(it->value == 5);
    it = it->next;
    common_ensure(it->value == 3);
    it = it->next;
    common_ensure(it->value == 4);
    
}

void test_singly_linked_list() {
    int i = 1;
    sll_t(int) *one = sll_alloc(one) != NULL ? one->value = i++, one : NULL;
    sll_t(int) *two = sll_alloc(two) != NULL ? two->value = i++, two : NULL;
    sll_t(int) *three = sll_alloc(three) != NULL ? three->value = i++, three : NULL;
    sll_t(int) *four = sll_alloc(four) != NULL ? four->value = i++, four : NULL;
    sll_t(int) *five = sll_alloc(five) != NULL ? five->value = i++, five : NULL;

    /* cons! */
    sll_push(one, sll_push(two, sll_push(three, sll_push(four, five))));

    {
        sll_t(int) *it = one;

        common_ensure(it->value == 1);
        it = it->next;
        common_ensure(it->value == 2);
        it = it->next;
        common_ensure(it->value == 3);
        it = it->next;
        common_ensure(it->value == 4);
        it = it->next;
        common_ensure(it->value == 5);
    }

    sll_destroy_all(one);
}

void test_hash_table() {
    int i;
    ht_t(double, int) ht;

    ht_make(&ht);
    {
        for (i = 0; i < 1024; i++) {
            ht_put_val(&ht, i, i);
        }

        for (i = 0; i < 1024; i++) {
            common_ensure(ht_get_val(&ht, i) != NULL);
        }


        {
            ht_link_entry_t(double, int) *it0, *head;
            size_t k = 0;
            ht_for_each(&ht, i, head, it0) k++;
            common_ensure(k == 1024);
        }

        {
            ht_entry_t(double, int) *it = ht_get_val(&ht, 45.0);

            common_ensure(it != NULL);
            common_ensure(it->value == 45);
        }
    }
    ht_destroy(&ht);
}

void test_hash_table_2() {
    int i;
    ht_t(const char *, int) ht;
    ht_entry_t(const char *, int) *it;
    ht_link_entry_t(const char *, int) *it0;
    vec(char *) str;
    vec_make(&str);

    ht_make(&ht);
    {
        for (i = 0; i < 16; i++) {
#define alloc_string(str) memcpy(calloc(1, 16), str, sizeof(str))
            char *test = alloc_string("test");
            vec_push(&str, test);
            char *abcd = alloc_string("abcd");
            vec_push(&str, abcd);
            char *hw = alloc_string("hello world");
            vec_push(&str, hw);
    
            char buf[16];
            sprintf(buf, "%i", i);
    
            strcat(test, buf);
            strcat(abcd, buf);
            strcat(hw, buf);
    
            ht_put_str(&ht, test, i);
            ht_put_str(&ht, abcd, i);
            ht_put_str(&ht, hw, i);
        }
        
        {
            it = ht_get_str(&ht, "abcd1");
            common_ensure(it != NULL);
            common_ensure(it->value == 1);
        }

        {
            it = ht_get_str(&ht, "test0");
            common_ensure(it != NULL);
            common_ensure(it->value == 0);
        }

        {
            it = ht_get_str(&ht, "hello world7");
            common_ensure(it != NULL);
            common_ensure(it->value == 7);
        }

        {
            it = ht_get_str_auto(&ht, "test1234");
            common_ensure(it != NULL);
            common_ensure(it->value == 0);
            common_ensure(ht_get_str(&ht, "test1234") != NULL);
        }

        {
            ht_del_str(&ht, "test1234");
            common_ensure(ht_get_str(&ht, "test1234") == NULL);
        }
    }

    ht_destroy(&ht);
    char **s;
    vec_for_each(&str, i, s) common_free((void *) *s);
    vec_destroy(&str);
}

void test_hash_table_3() {
    ht_t(int, int) table; /* create the table definition */
    ht_t(const char *, double) kv; /* string-number table */
    ht_make(&table); /* do this to initialize the fields */
    ht_make(&kv);

    ht_put_val(&table, 1234, 5678); /* drop in some value */
    ht_put_val(&table, 12345, 1234);
    ht_put_val(&table, 1561, 1734);

    common_ensure(ht_get_val(&table, 1234)->value == 5678);
    common_ensure(ht_get_val(&table, 12345)->value == 1234);
    common_ensure(ht_get_val(&table, 1561)->value == 1734);

    ht_del_val(&table, 12345);
    common_ensure(ht_get_val(&table, 12345) == NULL); /* NULL */

    ht_put_str(&kv, "the answer to life, the universe and everything", 42); /* add a string-int pair */
    ht_put_str(&kv, "e", 2.71828);
    ht_put_str(&kv, "pi", 3.14159);
    ht_put_str(&kv, "phi", 1.61803);

    common_ensure(ht_get_str(&kv, "the answer to life, the universe and everything")->value == 42.0); /* 42.0 (double representation) */
    common_ensure(ht_get_str(&kv, "e")->value == 2.71828); /* 2.71828 */
    common_ensure(ht_get_str(&kv, "pi")->value == 3.14159); /* 3.14159 */
    common_ensure(ht_get_str(&kv, "phi")->value == 1.61803); /* 1.61803 */

    ht_get_str_auto(&kv, "g")->value = 9.80665; /* if the field does not exist, automatically create one */
    common_ensure(ht_get_str(&kv, "g")->value == 9.80665); /* 9.80665 */

    ht_del_str(&kv, "e");
    common_ensure(ht_get_str(&kv, "e") == NULL); /* NULL */

    ht_destroy(&table); /* clean up the resources used */
    ht_destroy(&kv);
}

void test_json_serialize() {

    json j, j5, j6;


    json_make_number(&j5, 123.456);

    json_make_object(&j6);
    json_object_put(&j6, "test", &j5);

    json_make_object(&j);

    json_object_put(&j, "test1", &j6);


    size_t written;
    char buf[65535] = { 0 };
    json_serialize_ret ret = json_serialize(&j, buf, 65535, &written);
    if (ret == SERIAL_OK) {
        printf("%.*s\n", (int)written, buf);
    } else {
        printf("serialization failed\n");
    }

    json_destroy(&j);
}

void test_reference_counting() {
    rc_t(int) i;
    rc_make(&i);

    rc_inflate(&i);
    common_ensure(rc_count(&i) == 2);

    rc_deflate(&i);
    common_ensure(rc_count(&i) == 1);

    rc_inflate(&i);
    rc_inflate(&i);
    rc_inflate(&i);
    rc_enter(&i) {
        printf("%i\n", *rc_get(&i));
        common_ensure(rc_count(&i) == 4);
    }
    common_ensure(rc_count(&i) == 3);

    rc_deflate(&i);
    rc_deflate(&i);
    rc_deflate(&i);
    // rc_deflate(&i);
    common_ensure(rc_count(&i) == 0);
    common_ensure(rc_valid(&i) == 0);
}

int test_rc_2_signal = 0;
void test_reference_counting_2_fn(int *ptr) {
    test_rc_2_signal = 1;
}

void test_reference_counting_2() {
    char buf;

    rc_t(int) i;
    rc_pin_mem(&i, &buf, test_reference_counting_2_fn);

    rc_inflate(&i);
    rc_deflate(&i);
    rc_deflate(&i);

    common_ensure(rc_count(&i) == 0);
    common_ensure(rc_valid(&i) == 0);
    common_ensure(test_rc_2_signal == 1);
}

int main(void) {
    test_vector();
    test_linked_list();
    test_singly_linked_list();
    test_hash_table();
    test_hash_table_2();
    test_hash_table_3();
    test_reference_counting();
    test_reference_counting_2();

    /*test_json_serialize();*/
    printf("done\n");
    getchar();
    return 0;
}
