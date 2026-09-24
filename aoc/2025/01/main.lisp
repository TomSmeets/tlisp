(def parsenum (s) (do
  (let n 0)
  (while (not (nil? s)) (do
    (let x (car s))
    (set n (add (mul n 10) (sub x (car "0"))))
    (set s (cdr s))
  ))
  n
))

(def split (xs ys) (do
    (if (startswith ys xs))
))

(let file (readfile "2501.txt"))
(let lines (split "\n" file))
(let sum 50)
(let zero_count 0)
(for lines l (do
     (let dir (car l))
     (let inc (parse (cdr l)))
     (if (eq dir 'L') (set inc (sub 0 inc)))
     (set sum (add sum inc))

     ; Wrap around
     (while (gte sum 100) (set sum (sub sum 100)))
     (while (lt  sum   0) (set sum (add sum 100)))

     ; Increment number
     (if (eq sum 0) (set zero_count (add zero_count 1)))
 ))
