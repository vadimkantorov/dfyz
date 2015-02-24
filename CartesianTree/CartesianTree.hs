module CartesianTree (CTree, empty, member, insert, delete, fromList, height, count) where

import System.Random hiding (split)
import Data.List (sort)
import Test.QuickCheck

data Node a = Leaf | Node { value :: a, priority :: Int, left :: Node a, right :: Node a }
data CTree rnd a = CTree rnd (Node a)

instance Show a => Show (Node a)
	where
	show Leaf = "."
	show (Node k p left right) = "(" ++ (show left) ++ " " ++ (show k) ++ "/" ++ (show p) ++ " " ++ (show right) ++ ")"

instance Show a => Show (CTree rnd a)
	where
	show = show . extractNode

empty :: Int -> CTree StdGen a
empty seed = CTree (mkStdGen seed) Leaf

member :: (RandomGen rnd, Ord a) => CTree rnd a -> a -> Bool
member (CTree _ t) k = member' t k
	where
	member' Leaf _ = False
	member' (Node v _ left right) k
		| v == k = True
		| v < k = member' right k
		| v > k = member' left k

split :: Ord a => Node a -> a -> (Node a, Node a)
split Leaf k = (Leaf, Leaf)
split r @ (Node v _ left right) k =
	if v <= k
		then let (left', right') = split right k in (r { right = left' }, right')
		else let (left', right') = split left k in (left', r { left = right' })

merge :: Node a -> Node a -> Node a
merge x Leaf = x
merge Leaf x = x
merge r1@(Node _ p1 left1 right1) r2@(Node _ p2 left2 right2) =
	if p2 >= p1
		then r1 { right = merge right1 r2 }
		else r2 { left = merge r1 left2 }

insert :: (RandomGen rnd, Ord a) => CTree rnd a -> a -> CTree rnd a
insert (CTree rnd t) k = CTree rnd' t'
	where
	(np, rnd') = next rnd
	(left, right) = split t k
	nn = Node k np Leaf Leaf
	t' = merge (merge left nn) right

delete :: (RandomGen rnd, Ord a) => CTree rnd a -> a -> CTree rnd a
delete (CTree rnd t) k = CTree rnd (delete' t k)
	where
	delete' Leaf k = Leaf
	delete' r @ (Node val _ left right) k =
		if val == k
			then merge left right
			else r { left = delete' left k, right = delete' right k }

extractNode :: CTree rnd a -> Node a
extractNode (CTree _ t) = t

height' Leaf = 0
height' (Node _ _ left right) = 1 + max (height' left) (height' right)

height :: CTree rnd a -> Int
height = height' . extractNode

fromList :: Ord a => Int -> [a] -> CTree StdGen a
fromList seed xs = foldl insert (empty seed) xs

toList' Leaf = []
toList' (Node k _ left right) = toList' left ++ [k] ++ toList' right

toList :: Ord a => CTree rnd a -> [a]
toList = toList' . extractNode

count' Leaf = 0
count' (Node _ _ left right) = sum [1, count' left, count' right]

count = count' . extractNode

-- Tests for QuickCheck

checkHeap Leaf = True
checkHeap (Node _ p left right) = ok p left && ok p right
	where
	ok p Leaf = True
	ok p r = p <= priority r && checkHeap r

checkBst :: Ord a => Node a -> Bool
checkBst = ordered . toList'
	where
	ordered list = list == (sort list)
	
prop_IsHeap :: [Int] -> Int -> Bool
prop_IsHeap arr seed = checkHeap $ extractNode $ fromList seed arr

prop_IsBst :: [Int] -> Int -> Bool
prop_IsBst arr seed = checkBst $ extractNode $ fromList seed arr
