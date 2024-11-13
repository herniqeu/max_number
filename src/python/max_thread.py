import threading
import time
import random
import sys

class MaxThread(threading.Thread):
    """Thread class to calculate partial max of a list"""
    def __init__(self, numbers, start_index, end_index):
        threading.Thread.__init__(self)
        self.numbers = numbers
        self.start_index = start_index
        self.end_index = end_index
        self.partial_max = float('-inf')

    def run(self):
        """Calculate the max of the assigned portion of the list"""
        self.partial_max = max(self.numbers[self.start_index:self.end_index])

def parallel_max(numbers, num_threads=2):
    """
    Calculate the max of a list using multiple threads
    
    Args:
        numbers (list): List of numbers to find the max
        num_threads (int): Number of threads to use
        
    Returns:
        tuple: Total max and execution time
    """
    # Record start time
    start_time = time.time()
    
    # Calculate chunk size for each thread
    chunk_size = len(numbers) // num_threads
    
    # Create and start threads
    threads = []
    for i in range(num_threads):
        start_idx = i * chunk_size
        # Handle the last chunk which might be larger
        end_idx = len(numbers) if i == num_threads - 1 else (i + 1) * chunk_size
        
        thread = MaxThread(numbers, start_idx, end_idx)
        threads.append(thread)
        thread.start()
    
    # Wait for all threads to complete
    for thread in threads:
        thread.join()
    
    # Calculate total max from partial maxes
    total_max = max(thread.partial_max for thread in threads)
    
    # Calculate execution time
    execution_time = time.time() - start_time
    
    return total_max, execution_time

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <size> <num_threads>")
        sys.exit(1)

    size = int(sys.argv[1])
    num_threads = int(sys.argv[2])

    # Test cases
    test_cases = [
        [random.randint(1, 100) for _ in range(size)],  # Random numbers
        [1] * size  # Uniform numbers
    ]
    
    for i, numbers in enumerate(test_cases, 1):
        print(f"\nTest Case {i}:")
        print(f"List size: {len(numbers)}")
        
        # Calculate max using regular method for comparison
        start_time = time.time()
        regular_max = max(numbers)
        regular_time = time.time() - start_time
        print(f"Regular max: {regular_max}")
        print(f"Regular execution time: {regular_time:.4f} seconds")
        
        # Calculate max using parallel method
        parallel_result, parallel_time = parallel_max(numbers, num_threads)
        print(f"Parallel max: {parallel_result}")
        print(f"Parallel execution time: {parallel_time:.4f} seconds")
        print(f"Speed improvement: {(regular_time / parallel_time):.2f}x")

if __name__ == "__main__":
    main() 