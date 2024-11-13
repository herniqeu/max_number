module Main where

import Control.Concurrent
import Control.Monad
import Data.Time.Clock
import System.Environment
import System.Exit

-- Worker function to calculate partial max
maxWorker :: [Int] -> MVar Int -> IO ()
maxWorker numbers result = do
    let partialMax = maximum numbers
    putMVar result partialMax

-- Parallel max implementation
parallelMax :: [Int] -> Int -> IO (Int, Double)
parallelMax numbers numWorkers = do
    startTime <- getCurrentTime
    
    -- Create MVars for results
    results <- replicateM numWorkers newEmptyMVar
    
    -- Calculate chunk size and create workers
    let chunkSize = length numbers `div` numWorkers
        chunks = splitIntoChunks chunkSize numbers
    
    -- Start workers
    forM_ (zip chunks results) $ \(chunk, result) ->
        forkIO $ maxWorker chunk result
    
    -- Collect results
    partialMaxes <- mapM takeMVar results
    let totalMax = maximum partialMaxes
    
    endTime <- getCurrentTime
    let duration = realToFrac $ diffUTCTime endTime startTime
    
    return (totalMax, duration)

-- Helper function to split list into chunks
splitIntoChunks :: Int -> [a] -> [[a]]
splitIntoChunks _ [] = []
splitIntoChunks n xs = take n xs : splitIntoChunks n (drop n xs)

main :: IO ()
main = do
    args <- getArgs
    case args of
        [size, threads] -> do
            let size' = read size :: Int
                threads' = read threads :: Int
            numbers <- return $ [1..size']
            (max', time) <- parallelMax numbers threads'
            putStrLn $ "Parallel max: " ++ show max'
            putStrLn $ "Time: " ++ show time
        _ -> do
            putStrLn "Usage: max_thread <size> <threads>"
            exitFailure