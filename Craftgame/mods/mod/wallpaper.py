# coding=utf-8
import cge

_tps = 0

@cge.CGEMod("wallpaper")
class Mod:
    def rs(self, event):
        if event.name == "GuiScreenMainMenu":
            cge.renderer.setTexturingMode(1)
            cge.renderer.setColor(cge.Vec4(1, 1, 1, 1))
            cge.textureManager.bind(cge.Resource("wlp:res/bg.png"))
            cge.renderer.drawRect(0, 0, event.gui.width, event.gui.height)
            event.cancel()

    def init(self):
        cge.registerEventHandler(self, self.rs, "craftgame:render_screen_bg")
        cge.textureManager.load(cge.Resource("wlp:res/bg.png"), True)

Mod()
