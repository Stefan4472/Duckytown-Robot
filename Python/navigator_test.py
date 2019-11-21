from navigator import *
import time

def test_route(route):
	nav = Navigator(route)

	instruct = nav.get_next_instruction()
	print(instruct)

	while instruct != None:
		instruct = nav.get_next_instruction()
		print(instruct)


# test_route((1,5,10))
# start_time = time.time()
# for i in range(100):
# 	test_route((6,9))
# print(time.time() - start_time)