import cge
import logger
	
@cge.CGEMod("My mod", "mm", "1.0.0")
class MyMod:
	def __init__(self):  
		#cge.registerEventHandler(self, self.render_screen, "craftgame:render_screen_bg/pre") 
		cge.connectToServer("alex2772.ru", 24565)
def init():
	mod = MyMod()