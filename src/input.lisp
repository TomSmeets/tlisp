(let f (readfile (quote src/input.lisp)))
(set (car (cdr (cdr f))) 88)
f
