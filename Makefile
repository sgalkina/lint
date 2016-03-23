CPP_FLAGS=-O2 -Wall -Werror -std=c++11 --pedantic
COMP_FLAGS=-O2 -Werror -std=c++11 --pedantic

all: bin/main

test: bin/lint_compilation_check bin/main python_test

bin/main: bin/lint.o bin/main.o
	clang++ -o bin/main $(CPP_FLAGS) bin/main.o bin/lint.o

bin/main.o: src/main.cpp src/lint.h
	clang++ -o bin/main.o -c $(CPP_FLAGS) src/main.cpp

bin/lint.o: src/lint.cpp src/lint.h
	clang++ -o bin/lint.o -c $(CPP_FLAGS) src/lint.cpp

bin/lint_compilation_check: bin/lint.o bin/lint_compilation_check.o
	clang++ -o bin/lint_compilation_check $(COMP_FLAGS) bin/lint.o bin/lint_compilation_check.o

bin/lint_compilation_check.o: src/lint_compilation_check.cpp src/lint.h
	clang++ -o bin/lint_compilation_check.o -c $(COMP_FLAGS) src/lint_compilation_check.cpp

python_test: test.py
	python test.py
