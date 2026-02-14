# Компилятор C
CC = gcc
# Флаги компиляции: стандарт C11, предупреждения, оптимизация
CFLAGS = -std=c11 -Wall -Wextra -O2
# Флаги препроцессора: включить текущую директорию и путь к Check в поиск заголовков
CPPFLAGS = -I. -I/opt/homebrew/opt/check/include
# Библиотеки для линковки: check (тесты), математика, потоки + путь к библиотекам Check
LDFLAGS = -L/opt/homebrew/opt/check/lib
#LDLIBS = -lcheck -lm -lpthread
# For ubuntu we add a -lsubunit flag
LDLIBS = -lcheck -lsubunit -lm -lpthread


SRC_DIRS = . Arithmetic Comparison Convertors Other
TEST_DIR = tests
# Исходные файлы: все .c файлы в текущей директории

# Исходные файлы: все .c файлы в указанных директориях
SRC = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
# Объектные файлы: заменяем .c на .o в именах исходников
OBJ = $(SRC:.c=.o)
# Исходные файлы тестов: все .c файлы в папке tests/
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
# Объектные файлы тестов: заменяем .c на .o в именах тестов
TEST_OBJ = $(TEST_SRC:.c=.o)
# Имя статической библиотеки
LIB = s21_decimal.a
# Имя исполняемого файла тестов
BIN = tests_runner

# Фиктивная цель (не файл): все
.PHONY: all
# Основная цель: собрать библиотеку
all: $(LIB)

# Правило сборки библиотеки из объектных файлов
$(LIB): $(OBJ)
	# Создать статическую библиотеку с помощью archiver
	ar rcs $@ $^

# Правило компиляции обычных .c файлов в .o
%.o: %.c
	# Компилировать исходник в объектный файл
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Правило компиляции тестовых .c файлов в .o
tests/%.o: tests/%.c
	# Компилировать тестовый исходник в объектный файл
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Фиктивная цель: тестирование
.PHONY: test
# Сборка и запуск тестов
test: $(LIB) $(TEST_OBJ)
	# Линковать тесты с библиотекой в исполняемый файл
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $(BIN) $(TEST_OBJ) $(LIB) $(LDLIBS)
	# Запустить тесты
	./$(BIN)

# Фиктивная цель: покрытие кода
.PHONY: gcov_report
# Генерация отчета о покрытии кода тестами
gcov_report: clean
	# Пересобрать библиотеку с флагами покрытия
	$(MAKE) CFLAGS="$(CFLAGS) --coverage -O0 -g" $(LIB)
	# Пересобрать и запустить тесты с флагами покрытия
	$(MAKE) CFLAGS="$(CFLAGS) --coverage -O0 -g" test
	# Создать папку для отчета
	mkdir -p coverage
	# Сгенерировать HTML отчет о покрытии кода
	gcovr -r . --html --html-details -o coverage/index.html

# Фиктивная цель: очистка
.PHONY: clean
# Удаление всех сгенерированных файлов
clean:
	# Удалить все объектные файлы во всех поддиректориях
	rm -f $(foreach dir,$(SRC_DIRS),$(dir)/*.o)
	rm -f $(TEST_DIR)/*.o
	# Удалить файлы покрытия кода gcov
	rm -f $(foreach dir,$(SRC_DIRS),$(dir)/*.gcno $(dir)/*.gcda)
	rm -f *.gcno *.
	rm -f $(TEST_DIR)/*.gcno $(TEST_DIR)/*.gcda
	# Удалить отчеты gcov
	rm -f *.gcov
	# Удалить папку с отчетами о покрытии
	rm -rf coverage
	# Удалить объектные файлы, библиотеку и исполняемый файл
	rm -f $(OBJ) $(TEST_OBJ) $(BIN) $(LIB)