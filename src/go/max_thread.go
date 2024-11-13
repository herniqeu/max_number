package main

import (
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"sync"
	"time"
)

// MaxWorker calculates partial max for a slice of numbers
func MaxWorker(numbers []int, result chan<- int) {
	max := numbers[0]
	for _, num := range numbers {
		if num > max {
			max = num
		}
	}
	result <- max
}

// ParallelMax calculates max using multiple goroutines
func ParallelMax(numbers []int, numWorkers int) (int, time.Duration) {
	start := time.Now()

	chunkSize := len(numbers) / numWorkers
	results := make(chan int, numWorkers)
	var wg sync.WaitGroup

	// Start workers
	for i := 0; i < numWorkers; i++ {
		wg.Add(1)
		startIdx := i * chunkSize
		endIdx := startIdx + chunkSize
		if i == numWorkers-1 {
			endIdx = len(numbers)
		}

		go func(start, end int) {
			defer wg.Done()
			MaxWorker(numbers[start:end], results)
		}(startIdx, endIdx)
	}

	// Close results channel when all workers are done
	go func() {
		wg.Wait()
		close(results)
	}()

	// Calculate total max
	totalMax := numbers[0]
	for partialMax := range results {
		if partialMax > totalMax {
			totalMax = partialMax
		}
	}

	return totalMax, time.Since(start)
}

func main() {
	if len(os.Args) != 3 {
		fmt.Fprintf(os.Stderr, "Usage: %s <size> <num_threads>\n", os.Args[0])
		os.Exit(1)
	}

	size, _ := strconv.Atoi(os.Args[1])
	numThreads, _ := strconv.Atoi(os.Args[2])

	// Test cases
	testCases := [][]int{
		make([]int, size), // Random numbers
		make([]int, size), // Uniform numbers
	}

	// Initialize test cases
	for i := range testCases[0] {
		testCases[0][i] = rand.Intn(100) + 1
	}

	for i := range testCases[1] {
		testCases[1][i] = 1
	}

	// Run tests
	for i, numbers := range testCases {
		fmt.Printf("\nTest Case %d:\n", i+1)
		fmt.Printf("List size: %d\n", len(numbers))

		// Regular max
		start := time.Now()
		regularMax := numbers[0]
		for _, num := range numbers {
			if num > regularMax {
				regularMax = num
			}
		}
		regularTime := time.Since(start)

		fmt.Printf("Regular max: %d\n", regularMax)
		fmt.Printf("Regular time: %v\n", regularTime)

		// Parallel max
		parallelMax, parallelTime := ParallelMax(numbers, numThreads)
		fmt.Printf("Parallel max: %d\n", parallelMax)
		fmt.Printf("Parallel time: %v\n", parallelTime)
	}
}
