(def parsenum (s) (do
  (print "parsenum" s)
  (let n 0)
  (while (not (nil? s)) (do
    (print "s" s)
    (let x (car s))
    (set n (add (mul n 10) (sub x (car "0"))))
    (set s (cdr s))
    (print "n" n)
  ))
  n
))

(parsenum "12356")
