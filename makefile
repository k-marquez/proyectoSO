comp = clang++

main: supervisor.out
	./supervisor.out

supervisor.out: washleaves.out putdough.out putstew.out tieuphallaca.out
	$(comp) supervisor.cpp -o  supervisor.out

washleaves.out: washleaves.cpp
	$(comp) washleaves.cpp -o washleaves.out

putdough.out: putdough.cpp
	$(comp) putdough.cpp -o putdough.out

putstew.out: putstew.cpp
	$(comp) putstew.cpp -o putstew.out

tieuphallaca.out: tieuphallaca.cpp
	$(comp) tieuphallaca.cpp -o tieuphallaca.out

clean:
	rm -f *out