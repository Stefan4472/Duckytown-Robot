from enum import Enum
from networkx import DiGraph	
from networkx import shortest_path



class TurnType(Enum):
  LEFT = 1,
  RIGHT = 2,
  STRAIGHT = 3

  
class Navigator:
	def __init__(self, route):  # TODO: THIS IS JUST A SKELETON RIGHT NOW. ALWAYS RETURNS STRAIGHT
		self.graph = DiGraph()
		self.init_graph()
		self.route = route
		self.curr_instruction = 0 # instruction in the current route
		self.path_instruction = 0 # instruction in the current path (path is between each node in the route)
		self.get_next_path()


	def init_graph(self):
		edges_ = ((1,4,{'Turn': TurnType.LEFT, 'Speed': 10}),
				 (1,12,{'Turn': TurnType.STRAIGHT, 'Speed': 10}), 
				 (2,4,{'Turn': TurnType.RIGHT, 'Speed': 10}),
				 (2,8,{'Turn': TurnType.STRAIGHT, 'Speed': 10}), 
				 (3,8,{'Turn': TurnType.RIGHT, 'Speed': 10}),
				 (3,12,{'Turn': TurnType.LEFT, 'Speed': 10}), 
				 (4,7,{'Turn': TurnType.LEFT, 'Speed': 10}),
				 (4,11,{'Turn': TurnType.RIGHT, 'Speed': 10}), 
				 (5,3,{'Turn': TurnType.LEFT, 'Speed': 10}),
				 (5,7,{'Turn': TurnType.STRAIGHT, 'Speed': 10}), 
				 (6,3,{'Turn': TurnType.RIGHT, 'Speed': 10}),
				 (6,11,{'Turn': TurnType.STRAIGHT, 'Speed': 10}), 
				 (7,1,{'Turn': TurnType.LEFT, 'Speed': 10}),
				 (7,10,{'Turn': TurnType.STRAIGHT, 'Speed': 15}), 
				 (8,6,{'Turn': TurnType.RIGHT, 'Speed': 10}),
				 (8,10,{'Turn': TurnType.LEFT, 'Speed': 15}), 
				 (9,1,{'Turn': TurnType.RIGHT, 'Speed': 10}),
				 (9,6,{'Turn': TurnType.STRAIGHT, 'Speed': 10}), 
				 (10,2,{'Turn': TurnType.LEFT, 'Speed': 10}),
				 (10,5,{'Turn': TurnType.STRAIGHT, 'Speed': 10}), 
				 (11,2,{'Turn': TurnType.RIGHT, 'Speed': 10}),
				 (11,9,{'Turn': TurnType.STRAIGHT, 'Speed': 15}), 
				 (12,5,{'Turn': TurnType.LEFT, 'Speed': 10}),
				 (12,9,{'Turn': TurnType.RIGHT, 'Speed': 15}))
		self.graph.add_edges_from(edges_)


	def set_instruction(self, num):
		self.curr_instruction = num
		self.path = self.get_next_path()
		self.path_instruction = 0


	def get_next_path(self):
		curr = self.curr_instruction
		try:
			# print("NEW PATH from {} to {}".format(self.route[curr], self.route[curr+1]))
			self.path = shortest_path(self.graph, source=self.route[curr], target=self.route[curr+1])
			# print(self.path)
		except:
			return []


	def finished_path(self):
		return self.path_instruction >= len(self.path)-1


	def finished_route(self):
		return self.curr_instruction >= len(self.route)-1


	def get_next_instruction(self):
		# time.sleep(1)
		if self.finished_path():
			# print("finished_path")
			self.curr_instruction += 1
			if self.finished_route():
				return None
			self.get_next_path()
			self.path_instruction = 0


		start = self.path[self.path_instruction]
		end = self.path[self.path_instruction+1]
		self.path_instruction += 1

		edge_Data = self.graph.get_edge_data(start, end)
		return (edge_Data["Turn"], edge_Data["Speed"])
