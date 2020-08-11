dict_analysis:
	clang -O2 -std=c11 -o dict_analysis ./src/dict_analysis.c

clean:
	rm -f *.o dict_analysis

debug:
	clang -ggdb3 -O0 -std=c11 -o dict_analysis ./src/dict_analysis.c
