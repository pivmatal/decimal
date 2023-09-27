CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wextra
CLEAN=rm -Rf

uname_S := $(shell uname -s)
ifeq ($(uname_S), Linux)
	CHECK_LIBS=-lcheck -lm -lpthread -lsubunit -lrt
	LEAKS=valgrind $(BUILD_DIR)/test
else
	CHECK_LIBS=-lcheck -lm -lpthread
	LEAKS=CK_FORK=no leaks --atExit -- $(BUILD_DIR)/test
endif

SRC_DIR=.
TEST_DIR=./tests
BUILD_DIR=../build
SOURCES=*.c
HEADERS=*.h
OBJ=*.o

LIB_NAME=s21_decimal.a

all: clean test $(LIB_NAME)

clean:
	$(CLEAN) $(OBJ) *.a *.gcno *.gcda *.gcov *.info
	$(CLEAN) $(BUILD_DIR)/*

clean_all: clean
	$(CLEAN) .clang-format ./tests/.clang-format *.log *.txt
	rmdir $(BUILD_DIR)

$(LIB_NAME): $(SRC_DIR)/$(SOURCES) $(SRC_DIR)/$(HEADERS)
	$(CC) $(CFLAGS) -g -c $(SRC_DIR)/$(SOURCES)
	ar rc $(LIB_NAME) $(OBJ)
	$(CLEAN) $(OBJ)

# для запуска тестов
test: $(LIB_NAME)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -g $(TEST_DIR)/tests.c -L. $(LIB_NAME) $(CHECK_LIBS) -o $(BUILD_DIR)/test
	$(CC) $(CFLAGS) -g $(TEST_DIR)/tests_2.c -L. $(LIB_NAME) $(CHECK_LIBS) -o $(BUILD_DIR)/test_2
	$(BUILD_DIR)/test
	$(BUILD_DIR)/test_2

# покрытие кода тестами
gcov_report: clean
	$(CC) $(CFLAGS) --coverage -c $(SRC_DIR)/$(SOURCES)
	ar rc $(LIB_NAME) $(OBJ)
	$(CC) $(CFLAGS) $(TEST_DIR)/tests_2.c -L. $(LIB_NAME) $(CHECK_LIBS) -lgcov -o $(BUILD_DIR)/test_2
	$(CLEAN) $(OBJ)
	$(BUILD_DIR)/test_2
	gcov *.gcda
	lcov -t "gcov_report" -o gcov_report.info -c -d .
	mkdir -p $(BUILD_DIR)/report
	genhtml -o $(BUILD_DIR)/report gcov_report.info

# проверки
style:
	cp ../materials/linters/.clang-format .
	clang-format -n *.c *.h
	cp ../materials/linters/.clang-format $(TEST_DIR)
	clang-format -n $(TEST_DIR)/*.c $(TEST_DIR)/*.h

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --check-config *.c *.h

leaks:
	$(LEAKS)

valgrind :
	CK_FORK=no valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all ./$(BUILD_DIR)/test
	CK_FORK=no valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all ./$(BUILD_DIR)/test_2
