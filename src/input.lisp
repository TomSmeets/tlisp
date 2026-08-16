(do
  (let four 4)
  (let five (add four 1))
  (print four five)
  (env! ((four 1) (five 2)))
  (print four five)
  (env?)
)
