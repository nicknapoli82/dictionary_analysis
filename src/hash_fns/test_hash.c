#include "djb2.h"

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

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

double COLLISION_WEIGHT = 1.0;
double UNFILLED_WEIGHT = 1.0;
unsigned int N = 2;

void run_hash(char *dict[], bucket *table, unsigned int word_count);
void use_hashes(unsigned long *hashes, bucket *table, unsigned int word_count);
table_stats collect_tStats(bucket *table);
void print_t_stats(table_stats t_s, unsigned int word_count);

void test_hash(char *dict[], unsigned int word_count) {
    unsigned long *hashes = malloc(sizeof(unsigned long) * word_count);
    for(unsigned int i = 0; i < word_count; i++) {
	hashes[i] = djb2((unsigned char*)dict[i]);
    }

    double smoothing_distribution[10] = {0.0};
    double rate_of_change[10] = {0.0};
    double last_roc = DBL_MAX;
    
    table_stats optimal = {0, 0, 0, 0, 0, 1.0, 0.0};

    while(1) {
	bucket *table = calloc(sizeof(bucket), N);
//	run_hash(dict, table, word_count);
	use_hashes(hashes, table, word_count);
	
	table_stats t_s = collect_tStats(table);
	free(table);

	double sd_current = 0.0;
	double roc_current = 0.0;
	for(int push = (sizeof(smoothing_distribution) / sizeof(double)) - 2; push >= 0; push--) {
	    smoothing_distribution[push + 1] = smoothing_distribution[push];
	    rate_of_change[push + 1] = rate_of_change[push];
	    sd_current += smoothing_distribution[push];
	    if(rate_of_change[push] != 0.0) {
		roc_current += rate_of_change[push];
	    }
	}
	smoothing_distribution[0] = t_s.weighted_distribution;
	sd_current += smoothing_distribution[0];
	rate_of_change[0] = sd_current / 10;
	roc_current = (rate_of_change[0] + roc_current) / 10;
	if(!(N % 1000)) {
	    printf("Testing N %i | Current Distribution %f\n", N, sd_current);
	}	
	if(last_roc < roc_current && N > 15) {
	    if(t_s.weighted_distribution >= t_s.average_lookup) {
		optimal = t_s;
		break;
	    }
	}
	last_roc = roc_current;
	N++;
    }

    printf("\n\n");
    printf("Optimal N found is %i\n\n", N);
    print_t_stats(optimal, word_count);
}

void run_hash(char *dict[], bucket *table, unsigned int word_count) {
    for(unsigned int i = 0; i < word_count; i++) {
	unsigned long hash = djb2((unsigned char*)dict[i]);
	table[hash % N].collisions++;
    }
}

void use_hashes(unsigned long *hashes, bucket *table, unsigned int word_count) {
    for(unsigned int i = 0; i < word_count; i++) {
	table[hashes[i] % N].collisions++;
    }
}

table_stats collect_tStats(bucket *table) {
    table_stats t_s = {0, 0, 0, 0, 0, 1.0, 0.0};
    unsigned int total_collisions = 0;
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
	    total_collisions += table[i].collisions;
	}
    }
    t_s.average_lookup = (double)total_collisions / (double)(N - t_s.unfilled_buckets);

    // I'm weighting the distribution using useful distribution vs waste
    // Collisions past the first value in a bucket is waste
    // Buckets that are not filled are waste
    // A value closer to 1 is a better use of the table
    // A value of 1 is the best we are going to get
    // I believe this is correct
    double useful = (double)t_s.filled_no_collision + (double)t_s.filled_with_collision;
    double waste = ((double)t_s.total_collisions * COLLISION_WEIGHT) + ((double)t_s.unfilled_buckets * UNFILLED_WEIGHT);
    t_s.weighted_distribution = (useful - waste);
    t_s.weighted_distribution = t_s.weighted_distribution != 0.0 ? (double)N / t_s.weighted_distribution : 1.0;
    t_s.weighted_distribution = t_s.weighted_distribution > 0 ? t_s.weighted_distribution : t_s.weighted_distribution * -1.0;
    t_s.weighted_distribution = t_s.weighted_distribution < 1 ? 1.0 / t_s.weighted_distribution : t_s.weighted_distribution;

    return t_s;
}

void print_t_stats(table_stats t_s, unsigned int word_count) {
    printf("For %i words. The table has\n", word_count);
    printf("%i total collisions\n", t_s.total_collisions);
    printf("%i buckets with length 1\n", t_s.filled_no_collision);
    printf("%i buckets with collisions\n", t_s.filled_with_collision);
    printf("%i unfilled buckets\n", t_s.unfilled_buckets);
    printf("%i is the longest bucket\n", t_s.most_collisions);
    printf("%2f average bucket length\n", t_s.average_lookup);
    printf("%2f weighted distribution\n", t_s.weighted_distribution);    
}
