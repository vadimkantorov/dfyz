module OrderedList (OrderedList, ordered, create, toList, insert, union, intersect, diff) where

data Ord a => OrderedList a = OrderedList [a] deriving Show

ordered :: Ord a => [a] -> Bool
ordered list = and (zipWith (<) list (drop 1 list))

create :: Ord a => [a] -> OrderedList a
create list =
	if ordered list
		then OrderedList list
		else error "OrderedList.create: unordered list"

toList :: Ord a => OrderedList a -> [a]
toList (OrderedList list) = list

combine x y f ex ey = create $ combine' (toList x) (toList y) f ex ey
	where
	combine' x y f ex ey = case (x, y, f, ex, ey) of
		([], y, _, ex, _) -> ex y
		(x, [], _, _, ey) -> ey x
		(x, y, f, ex, ey) -> z ++ (combine' x' y' f ex ey)
		where (z, x', y') = f x y

insert :: Ord a => OrderedList a -> a -> OrderedList a
insert (OrderedList list) key = create $ insert' list key
	where
	insert' [] key = [key]
	insert' l@(x:xs) key
		| key < x = key:l
		| key > x = x:(insert' xs key)
		| key == x = l

union :: Ord a => OrderedList a -> OrderedList a -> OrderedList a
union x y = combine x y f id id
	where
	f l1@(x:xs) l2@(y:ys)
		| x < y = ([x], xs, l2)
		| x > y = ([y], l1, ys)
		| x == y = ([x], xs, ys)

intersect :: Ord a => OrderedList a -> OrderedList a -> OrderedList a
intersect x y = combine x y f (const []) (const [])
	where
	f l1@(x:xs) l2@(y:ys)
		| x < y = ([], xs, l2)
		| x > y = ([], l1, ys)
		| x == y = ([x], xs, ys)

diff :: Ord a => OrderedList a -> OrderedList a -> OrderedList a
diff x y = combine x y f (const []) id
	where
	f l1@(x:xs) l2@(y:ys)
		| x < y = ([x], xs, l2)
		| x > y = ([], l1, ys)
		| x == y = ([], xs, ys)