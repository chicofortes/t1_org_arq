all:
	@gcc main.c handle_file.c -I. -o main
run:
	./main
