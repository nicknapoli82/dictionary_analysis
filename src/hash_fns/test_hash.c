#include "djb2.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unsigned int collisions;
} bucket;

typedef struct {
    unsigned int total_collisions;
    unsigned int filled_no_collision;
    unsigned int filled_with_collision;
    unsigned int unfilled_buckets;
    unsigned int most_collisions;
    double       average_lookup;
    double       weighted_distribution;
} table_stats;

#define N (250000)

void test_hash(char *dict[], unsigned int word_count) {
    bucket *table = calloc(sizeof(bucket), N);

    for(unsigned int i = 0; i < word_count; i++) {
	unsigned long hash = djb2((unsigned char*)dict[i]);
	table[hash % N].collisions++;
    }

    table_stats t_s = {0, 0, 0, 0, 0, 1.0, 0.0};
    for(unsigned int i = 0; i < N; i++) {
	if(table[i].collisions == 0) {
	    t_s.unfilled_buckets++;
	}
	else if(table[i].collisions == 1) {
	    t_s.filled_no_collision++;
	}
	else if(table[i].collisions > 1){
	    t_s.total_collisions += table[i].collisions - 1;
	    t_s.filled_with_collision++;
	    if(table[i].collisions > t_s.most_collisions) {
		t_s.most_collisions = table[i].collisions;
	    }
	}
	if(table[i].collisions != 0) {
	    t_s.average_lookup = (t_s.average_lookup + (double)table[i].collisions) / 2.0;
	}
    }

    free(table);

    // I'm weighting the distribution using useful distribution vs waste
    // Collisions past the first value in a bucket is waste
    // Buckets that are not filled are waste
    // A value closer to 1 is a better use of the table
    // A value of 1 is a perfect hash function
    // I believe this is correct
    double useful = (double)t_s.filled_no_collision + (double)t_s.filled_with_collision;
    double waste = (double)t_s.total_collisions + (double)t_s.unfilled_buckets;
    t_s.weighted_distribution = (useful - waste);
    t_s.weighted_distribution = t_s.weighted_distribution != 0.0 ? (double)N / t_s.weighted_distribution : 1.0;
    t_s.weighted_distribution = t_s.weighted_distribution > 0 ? t_s.weighted_distribution : t_s.weighted_distribution * -1.0;
    t_s.weighted_distribution = t_s.weighted_distribution < 1 ? 1.0 / t_s.weighted_distribution : t_s.weighted_distribution;
    printf("For %i words. The table has\n", word_count);
    printf("%i total collisions\n", t_s.total_collisions);
    printf("%i buckets with length 1\n", t_s.filled_no_collision);
    printf("%i buckets with collisions\n", t_s.filled_with_collision);
    printf("%i unfilled buckets\n", t_s.unfilled_buckets);
    printf("%i is the longest bucket\n", t_s.most_collisions);
    printf("%2f average bucket length\n", t_s.average_lookup);
    printf("%2f weighted distribution\n", t_s.weighted_distribution);
}
