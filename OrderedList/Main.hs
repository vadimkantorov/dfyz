module Main (main) where

import OrderedList
import qualified Data.List
import Test.QuickCheck

sortAndNub :: (Ord a) => [a] -> [a]
sortAndNub = Data.List.nub . Data.List.sort

instance (Arbitrary a, Ord a) => Arbitrary (OrderedList a)
	where
	arbitrary = do
		list <- arbitrary
		return $ create $ sortAndNub $ list

equivalent :: Ord a => ([a] -> [a] -> [a]) -> (OrderedList a -> OrderedList a -> OrderedList a) -> OrderedList a -> OrderedList a -> Bool
equivalent f g x y = (sortAndNub $ f (toList x) (toList y)) == (toList $ g x y)

prop_Union :: OrderedList Int -> OrderedList Int -> Bool
prop_Union = equivalent Data.List.union union

prop_Intersect :: OrderedList Int -> OrderedList Int -> Bool
prop_Intersect = equivalent Data.List.intersect intersect

prop_Diff :: OrderedList Int -> OrderedList Int -> Bool
prop_Diff = equivalent (Data.List.\\) diff

prop_Insert :: OrderedList Int -> Int -> Bool
prop_Insert list x = ordered $ toList $ insert list x

main = do
	quickCheck prop_Union
	quickCheck prop_Intersect
	quickCheck prop_Diff
	quickCheck prop_Insert