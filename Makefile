CCAnr: cca.cpp cca.h basis.h cw.h preprocessor.h
	g++ cca.cpp -O3 -march=native -mtune=native -flto -ffast-math -funroll-loops -finline-functions -static -DNDEBUG -o CCAnr

# Alternative debug build
debug: cca.cpp cca.h basis.h cw.h preprocessor.h
	g++ cca.cpp -O2 -g -static -o CCAnr_debug

# Clean target
clean:
	rm -f CCAnr CCAnr_debug

.PHONY: clean debug
