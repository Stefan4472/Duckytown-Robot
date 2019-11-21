from navigator import *
import time

# G_ = DiGraph()
# edges = ((1,2,{'attr': -1}), (2,3,{'attr': -2}))
# G_.add_edges_from(edges)
# print(shortest_path(G_, 1, 3))

def test_route(route):
	nav = Navigator(route)

	instruct = nav.get_next_instruction()
	print(instruct)

	while instruct != None:
		instruct = nav.get_next_instruction()
		print(instruct)

start_time = time.time()
for i in range(100):
	test_route((6,9))
print(time.time() - start_time)

# test_route((1,12,6))
