from enum import Enum

class TurnType(Enum):
  LEFT = 1,
  RIGHT = 2,
  STRAIGHT = 3
  
class Navigator:
  def __init__(self):  # TODO: THIS IS JUST A SKELETON RIGHT NOW. ALWAYS RETURNS STRAIGHT
    self.route_finished = False
    return
    
  def get_next_instruction(self):
    return TurnType.STRAIGHT
