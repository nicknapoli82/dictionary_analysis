#include "./hash_fns/test_hash.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
	printf("You effed up dude. What dictionary are we looking at?\n");
	printf("./dict_analysis <dictionary>\n");
	return 1;
    }
    
    unsigned int word_count = 0;

    FILE *file = fopen(argv[1], "r");
    fseek(file, 0, SEEK_END);
    size_t f_size = ftell(file);
    rewind(file);

    char *dict = malloc(f_size);
    fread(dict, 1, f_size, file);
    fclose(file);

    for(unsigned int i = 0; i < f_size; i++) {
	if(dict[i] == '\n') {
	    dict[i] = '\0';
	    word_count++;
	}
    }

    char *word_pointers[word_count];
    unsigned int max_length = 0;
    for(unsigned int i = 0, w = 0; i < f_size; i++, w++) {
	unsigned int len = 0;
	word_pointers[w] = &dict[i];
	while(dict[i] != '\0') {
	    len++;
	    i++;
	}
	if(len > max_length)
	    max_length = len;
    }

    unsigned int counts[max_length];
    memset(counts, 0, sizeof(unsigned int) * max_length);

    for(unsigned int i = 0; i < word_count - 1; i++) {
	for(unsigned int w = 1; w < max_length; w++) {
	    if(strncmp(word_pointers[i], word_pointers[i + 1], w) == 0)
		counts[w]++;
	    else break;
	}
    }

    unsigned int average_word_length = 0;
    unsigned int len_counts[max_length];
    memset(len_counts, 0, sizeof(unsigned int) * max_length);
    for(unsigned int i = 0; i < word_count; i++) {
	unsigned int w_len = strlen(word_pointers[i]);
	average_word_length += w_len;
	len_counts[w_len]++;
    }
    average_word_length = (float)average_word_length / (float)word_count;

    printf("The number of words in the dictionary is %i\n\n", word_count);
    printf("The average length of a dictionary word is %i\n", average_word_length);
    printf("The max word length is %i\n\n", max_length);
    for(unsigned int i = 1; i < max_length; i++) {
	printf("There %s %i (%c%f) %s with %i common beginning letters (%i words)\n",
	       counts[i] == 1 ? "is" : "are",
	       counts[i], '%',  ((float)counts[i] / (float)word_count) * 100,
	       counts[i] == 1 ? "word" : "words",
	       i,
	       len_counts[i]);
	if(counts[i] == 0)
	    break;
    }

    test_hash(word_pointers, word_count);

    // Not that I really care, but we may as well clean things up eh?
    free(dict);
}
