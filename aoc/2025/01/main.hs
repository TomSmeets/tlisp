read_move :: String -> Int
read_move ('L':xs) = -read xs
read_move ('R':xs) =  read xs

accum :: Int -> Int -> Int
accum x y = mod (x + y) 100

foldx :: (a -> b -> b) -> b -> [a] -> [b]
foldx f a (x:xs) = let b = f x a in b:foldx f b xs
foldx _ _ [] = []

dofile :: String -> Int
dofile =  length . filter (==0) . foldx accum 50 . map read_move . lines

main = readFile "2501.txt" >>= print . length . filter (==0) . foldx accum 50 . map read_move . lines
