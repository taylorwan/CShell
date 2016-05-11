make::
	gcc cshell.c -o shell
	gcc waiter.c -o waiter

clean::
	rm shell
	rm waiter
