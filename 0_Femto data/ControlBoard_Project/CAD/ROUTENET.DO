select net OUT12 
grid wire 0.000000 (direction x) (offset 0.000000)
grid wire 0.000000 (direction Y) (offset 0.000000)
grid via 0.000000 (direction x) (offset 0.000000)
grid via 0.000000 (direction Y) (offset 0.000000)
set route_diagonal 0
cost via -1
limit outside -1

write routes $/SpecctraWithinLayout.rte
quit -c