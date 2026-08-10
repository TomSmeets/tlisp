(let thing
  (fn (x y) (print x y))
  (let four 4
    (let five (+ 4 1)
      (do
        (thing 1 2)
        (thing four five)
        (print (quote Thing is) thing)
        (print (quote Env is) env)
      )
    )
  )
)
