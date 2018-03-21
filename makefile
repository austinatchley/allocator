.DEFAULT_GOAL := all
CFLAGS := 

# Feature detection
ifneq (, $(shell which g++-7))
	CXX := g++-7 # for travis
else
	CXX := g++   # for elsewhere
endif

ifneq (, $(shell which gcov-7))
	GCOV := gcov-7 # for travis
else
	GCOV := gcov   # for elsewhere
endif

ifneq (, $(shell which clang-format))
	CLANG_FORMAT := clang-format      # for travis
else
	CLANG_FORMAT := clang-format-3.5  # for elsewhere
endif

FILES :=                                  \
	.gitignore                            \
	Allocator.h                             \
	Allocator.log                           \
	html                                  \
	makefile                              \
	RunAllocator.c++                        \
	RunAllocator.in                         \
	TestAllocator.c++                       
#	RunAllocator.out
#	allocator-tests                         \
#	.travis.yml                           \
#	allocator-tests/austinatchley-RunAllocator.in  \
#	allocator-tests/austinatchley-RunAllocator.out

allocator-tests:
	git clone https://github.com/cs371pt-spring-2018/allocator-tests.git

html: Doxyfile Allocator.h
	doxygen Doxyfile

Allocator.log:
	git log > Allocator.log

# you must edit Doxyfile and
# set EXTRACT_ALL     to YES
# set EXTRACT_PRIVATE to YES
# set EXTRACT_STATIC  to YES
Doxyfile:
	doxygen -g

RunAllocator: Allocator.h RunAllocator.c++
	-clang-check -extra-arg=-std=c++14			   	Allocator.hh      --
	-clang-check -extra-arg=-std=c++14 -analyze	Allocator.hh      --
	-clang-check -extra-arg=-std=c++14          RunAllocator.c++  --
	-clang-check -extra-arg=-std=c++14 -analyze RunAllocator.c++  --
	$(CXX) $(CFLAGS) -pedantic -std=c++14 -Wall -Weffc++ -Wextra RunAllocator.c++ -o RunAllocator

RunAllocator.c++x: RunAllocator
	./RunAllocator < RunAllocator.in > RunAllocator.tmp
	-diff RunAllocator.tmp RunAllocator.out

TestAllocator: Allocator.h TestAllocator.c++
	-clang-check -extra-arg=-std=c++14          TestAllocator.c++ --
	-clang-check -extra-arg=-std=c++14 -analyze TestAllocator.c++ --
	$(CXX) $(CFLAGS) -fprofile-arcs -ftest-coverage -pedantic -std=c++14 -Wall -Weffc++ -Wextra TestAllocator.c++ -o TestAllocator -lgtest -lgtest_main -pthread

TestAllocator.c++x: TestAllocator
	valgrind ./TestAllocator
	$(GCOV) -b TestAllocator.c++ | grep -A 5 "File '.*Allocator.h'"
	$(GCOV) -b TestAllocator.c++ | grep -A 5 "File '.*TestAllocator.c++'"

all: RunAllocator TestAllocator

check: $(FILES)

clean:
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f *.plist
	rm -f *.tmp
	rm -f RunAllocator
	rm -f TestAllocator
	rm -f vgcore*
	rm -f doxygen_sqlite3.db
	rm -rf latex

config:
	git config -l

docker:
	docker run -it -v $(PWD):/usr/allocator -w /usr/allocator gpdowning/gcc

format:
	$(CLANG_FORMAT) -i Allocator.h
	$(CLANG_FORMAT) -i RunAllocator.c++
	$(CLANG_FORMAT) -i TestAllocator.c++

run: RunAllocator.c++x TestAllocator.c++x

scrub:
	make clean
	rm -f  Allocator.log
	rm -f  Doxyfile
	rm -rf allocator-tests
	rm -rf html
	rm -rf latex

status:
	make clean
	@echo
	git branch
	git remote -v
	git status

travis: allocator-tests html Allocator.log
	make clean
	ls -al
	make
	ls -al
	make run
	ls -al
	make -r check
