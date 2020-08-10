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

    for(unsigned int i = 0, w = 0; i < f_size; i++, w++) {
	word_pointers[w] = &dict[i];
	while(dict[i] != '\0') i++;
    }

    int counts[45] = {0};
    for(unsigned int i = 0; i < word_count - 1; i++) {
	for(unsigned int w = 2; w < 46; w++) {
	    if(strncmp(word_pointers[i], word_pointers[i + 1], w) == 0)
		counts[w]++;
	    else break;
	}
    }

    unsigned int average_word_length = 0;
    for(unsigned int i = 0; i < word_count; i++) {
	average_word_length += strlen(word_pointers[i]);
    }
    average_word_length = (float)average_word_length / (float)word_count;

    printf("The number of words in the dictionary is %i\n\n", word_count);
    printf("The average length of a dictionary word is %i\n\n", average_word_length);

    for(int i = 2; i < 45; i++) {
	printf("There %s %i (%c%f) %s with %i common beginning letters\n",
	       counts[i] == 1 ? "is" : "are",
	       counts[i], '%',  ((float)counts[i] / (float)word_count) * 100,
	       counts[i] == 1 ? "word" : "words",
	       i);
	if(counts[i] == 0)
	    break;
    }

    test_hash(word_pointers, word_count);
}
