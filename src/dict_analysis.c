#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
	printf("You effed up dude. What dictionary are we looking at?\n");
	printf("./dict_analysis <dictionary>\n");
	return 1;
    }
    
    unsigned int dict_words = 0;

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
	    dict_words++;
	}
    }

    char *word_pointers[dict_words];

    for(unsigned int i = 0, w = 0; i < f_size; i++, w++) {
	word_pointers[w] = &dict[i];
	while(dict[i] != '\0') i++;
    }

    int counts[45] = {0};
    for(unsigned int i = 0; i < dict_words - 1; i++) {
	for(unsigned int w = 2; w < 46; w++) {
	    if(strncmp(word_pointers[i], word_pointers[i + 1], w) == 0)
		counts[w]++;
	    else break;
	}
    }

    printf("The number of words in the dictionary is %i\n\n", dict_words);

    for(int i = 2; i < 45; i++) {
	printf("There %s %i (%c%f) %s with %i common beginning letters\n",
	       counts[i] == 1 ? "is" : "are",
	       counts[i], '%',  ((float)counts[i] / (float)dict_words) * 100,
	       counts[i] == 1 ? "word" : "words",
	       i);
	if(counts[i] == 0)
	    break;
    }
}
