#include "djb2.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unsigned int collisions;
} bucket;

typedef struct {
    unsigned int total_collisions;
    unsigned int unfilled_buckets;
    unsigned int most_collisions;
    unsigned int filled_no_collision;
    float        average_lookup;
    float        weighted_distribution;
} table_stats;

#define N 100000

void test_hash(char *dict[], unsigned int word_count) {
    bucket *table = calloc(sizeof(bucket), N);

    for(unsigned int i = 0; i < word_count; i++) {
	unsigned int hash = djb2((unsigned char*)dict[i]);
	table[hash % N].collisions++;
    }

    table_stats t_s = {0, 0, 0, 0, 1.0, 0.0};
    for(unsigned int i = 0; i < N; i++) {
	if(table[i].collisions == 0) {
	    t_s.unfilled_buckets++;
	}
	else if(table[i].collisions == 1) {
	    t_s.filled_no_collision++;
	}
	else if(table[i].collisions > 1){
	    t_s.total_collisions += table[i].collisions - 1;
	    if(table[i].collisions > t_s.most_collisions) {
		t_s.most_collisions = table[i].collisions;
	    }
	}
	if(table[i].collisions != 0) {
	    t_s.average_lookup = (t_s.average_lookup + (float)table[i].collisions) / 2.0;
	}
    }

    free(table);

    // I'm weighting the distribution against unfilled buckets
    // So having unfilled buckets is worse than having more collisions
    // on average.
    if(t_s.unfilled_buckets == 0) {
	t_s.weighted_distribution = (float)t_s.most_collisions / t_s.average_lookup;
    }
    else {
	float distribution = (float)(word_count - t_s.total_collisions - t_s.filled_no_collision) / t_s.average_lookup;
	t_s.weighted_distribution = ((float)t_s.unfilled_buckets / (float)N) * distribution;
	printf("distribution is %f\n", (float)t_s.unfilled_buckets / (float)N);
	t_s.weighted_distribution += t_s.average_lookup;
    }

    printf("For %i words. The table has\n", word_count);
    printf("%i total collisions\n", t_s.total_collisions);
    printf("%i buckets with length 1\n", t_s.filled_no_collision);
    printf("%i unfilled buckets\n", t_s.unfilled_buckets);
    printf("%i is the longest bucket\n", t_s.most_collisions);
    printf("%2f average bucket length\n", t_s.average_lookup);
    printf("%2f weighted distribution\n", t_s.weighted_distribution);
}
