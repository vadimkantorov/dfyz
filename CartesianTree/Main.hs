module Main (main) where

import CartesianTree
import System.CPUTime
import System.Random
import Text.Printf

getRandomSeed :: IO Int
getRandomSeed = getStdRandom $ random

inc :: Int -> IO [Int]
inc x = return $ [1..x]

dec :: Int -> IO [Int]
dec x = return $ [x, x-1..1]

rnd :: Int -> IO [Int]
rnd x = do
	gen <- getStdGen
	let list = randoms gen :: [Int]
	return $ take x list

treeHeight :: (Int -> IO [Int]) -> Int -> IO Int
treeHeight f nodes = do
	seed <- getRandomSeed
	list <- f nodes
	return $ height $ fromList seed list

timeAndHeight nodes f = do
	t1 <- getCPUTime
	h <- treeHeight f nodes
	h `seq` return h -- To force strict evaluation
	t2 <- getCPUTime
	let diff = (fromIntegral (t2 - t1)) / (10^12 :: Double)
	printf "%d %.3f " h diff

measure funcs nodes = do
	printf "%d " nodes
	mapM_ (timeAndHeight nodes) funcs
	printf "\n"

main = mapM_ (measure [inc, dec, rnd]) [1000, 2000..50000]
