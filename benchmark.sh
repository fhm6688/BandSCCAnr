#!/bin/bash

# Simple benchmark script for CCAnr performance testing
echo "Building optimized CCAnr..."
make clean
make

if [ ! -f "CCAnr" ]; then
    echo "Build failed!"
    exit 1
fi

echo "CCAnr built successfully with optimizations:"
echo "- O3 optimization with native architecture tuning"
echo "- Fast math lookup tables for sqrt, log, log2"
echo "- Fast xorshift random number generator"
echo "- Aligned memory allocation for cache optimization"
echo "- Branch prediction hints and prefetch instructions"
echo ""

echo "Performance optimizations applied:"
echo "1. Compilation flags: -O3 -march=native -mtune=native -flto -ffast-math -funroll-loops"
echo "2. Mathematical function lookup tables for hot paths"
echo "3. Fast random number generation (xorshift algorithm)"
echo "4. 64-byte aligned memory allocation for cache lines"
echo "5. Branch prediction hints for likely/unlikely paths"
echo "6. Memory prefetch hints for better cache utilization"
echo "7. Optimized loop patterns in critical sections"
echo ""

echo "Binary information:"
ls -lh CCAnr
echo ""

echo "To test with a SAT instance:"
echo "./CCAnr <instance.cnf> <seed> <p_first> <Beta_first> <total_p>"
echo ""
echo "Example: ./CCAnr example.cnf 1234 0.3 100 0.8"