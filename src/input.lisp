(def parsenum (s) (do
    (print "parsenum")
    (let n 0)
    (while (not (nil? s)) (do
       (let x (car s))
       (set n (add (mul n 10) (sub x '0')))
       (set s (cdr s))
    ))
    n
))
