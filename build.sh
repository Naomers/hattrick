gcc -g -Iinclude src/httoken_mem.c src/httoken.c src/htast_print.c src/htast_mem.c src/htast.c -fsanitize=address
