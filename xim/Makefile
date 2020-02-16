all: overspot onspot im_test xim_eterm ecore_test

overspot: overspot.c
	gcc -lX11 overspot.c -o overspot

onspot: onspot.c
	gcc -lX11 onspot.c -o onspot

im_test: im_test.c
	gcc -lX11 im_test.c -o im_test

xim_eterm: xim_eterm.c
	gcc -lX11 -g xim_eterm.c -o xim_eterm

ecore_test: xim_ecore.c
	gcc `pkg-config --libs --cflags ecore`  `pkg-config --libs --cflags ecore-x` xim_ecore.c -o ecore_test

clean:
	rm -f overspot onspot im_test xim_eterm ecore_test
