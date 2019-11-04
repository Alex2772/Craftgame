# coding=utf-8
import logger
import cge
import threading
import time
import random
import math


_tps = 0

@cge.CGEMod("mod")
class Mod:
    gen = False
    win = cge.Pos(0, 0, 0)

    def next_round(self):
        pre = cge.Pos(-2, 54, -6)
        for p in cge.getPlayers():
            p.getPlayer().setPos(cge.DPos(-1.5, 55, -5.5))
            p.getPlayer().setLook(0, 0)
        cge.getWorlds()[0].setBlock(cge.TileEntity(cge.getBlock(cge.Resource("craftgame:blocks/gold"))), pre)
        for x in range(-8, 4):
            for z in range(-40, -7):
                for y in range(51, 71):
                    cge.getWorlds()[0].setBlock(cge.TileEntity(cge.getBlock(cge.Resource("craftgame:blocks/air"))), cge.Pos(x, y, z))

        for i in range(0, 10):
            pre.z -= random.randint(2, 3)
            pre.y += random.randint(0, 1)
            pre.x += random.randint(-1, 1)
            cge.getWorlds()[0].setBlock(cge.TileEntity(cge.getBlock(cge.Resource("craftgame:blocks/stone"))), pre)

        self.win.x = pre.x + random.randint(-1, 1)
        self.win.y = pre.y + 1
        self.win.z = pre.z - 4

        for x in range(-1, 2):
            for z in range(-1, 2):
                temp = cge.Pos(self.win.x + x, self.win.y, self.win.z + z)
                block = cge.getBlock(cge.Resource("craftgame:blocks/stone"))
                if x == 0 and z == 0:
                    block = cge.getBlock(cge.Resource("craftgame:blocks/gold"))
                cge.getWorlds()[0].setBlock(cge.TileEntity(block), temp)


    def tps_timer(self):
        global _tps
        while 1:
            time.sleep(1)
            tps = _tps
            _tps = 0
            logger.info(str(tps))

    t = None

    def place(self, event):
        gp = event.player.getNetHandler().commandSender.getPlayer().getGameProfile()
        pos = event.player.getNetHandler().commandSender.getPlayer().getPos()
        s = "{} {} {}, by {} staying at {} {} {}".format(event.pos.x, event.pos.y, event.pos.z, gp.getUsername(), pos.x, pos.y, pos.z)
        event.player.getNetHandler().commandSender.sendMessage(s)
        logger.info(s)
        pos = event.pos
        #event.player.setPos(cge.DPos(pos.x + 0.5, pos.y + 1, pos.z + 0.5))

    def tick(self, event):
        global _tps
        _tps = _tps + 1

        if not self.gen:
            self.gen = True
            self.next_round()

    def move(self, event):
        e = event.entity.getPos().round()
        e.y -= 1
        if e.x == self.win.x and e.y == self.win.y and e.z == self.win.z:
            event.entity.getNetHandler().commandSender.sendMessage("§2Красава!")
            cge.chat.broadcast("§e{} §6победил!".format(event.entity.getGameProfile().getUsername()))
            self.next_round()
        elif event.entity.getPos().y <= 51:
            event.entity.setPos(cge.DPos(-1.5, 55, -5.5))
            event.entity.setLook(0, 0)
            event.entity.getNetHandler().commandSender.sendMessage("§4Лох!")
        else:
            p = cge.Pos(math.floor(event.entity.getPos().x), math.floor(event.entity.getPos().y - 1), math.floor(event.entity.getPos().z))
            if cge.getWorlds()[0].getBlock(p).getBlock().getResource().full == "craftgame:blocks/stone":
                cge.getWorlds()[0].setBlock(cge.TileEntity(cge.getBlock(cge.Resource("craftgame:blocks/gold"))), p)

        """

        p = cge.Pos(math.floor(event.entity.getPos().x), math.floor(event.entity.getPos().y), math.floor(event.entity.getPos().z))
        for x in range(-2, 2):
            for z in range(-2, 2):
                for y in range(-2, 4):
                    cge.getWorlds()[0].setBlock(cge.TileEntity(cge.getBlock(cge.Resource("craftgame:blocks/air"))), cge.Pos(p.x + x, p.y + y, p.z + z))

        """
    def chat(self, event):
        #event.format = "§f{}§7: {}".format(event.player.getGameProfile().getUsername(), event.message)
        pass

    def rs(self, event):
        if event.name == "GuiScreenMainMenu":
            cge.renderer.setTexturingMode(1)
            cge.renderer.setColor(cge.Vec4(1, 1, 1, 1))
            cge.textureManager.bind(cge.Resource("mod:res/bg.png"))
            cge.renderer.drawRect(0, 0, event.gui.width, event.gui.height)
            event.cancel()

    def init(self):
        cge.registerEventHandler(self, self.place, "craftgame:place_block")
        cge.registerEventHandler(self, self.tick, "craftgame:tick")
        cge.registerEventHandler(self, self.move, "craftgame:move")
        cge.registerEventHandler(self, self.chat, "craftgame:chat")
        cge.registerEventHandler(self, self.rs, "craftgame:render_screen_bg")
        cge.textureManager.load(cge.Resource("mod:res/bg.png"), True)
        global t
        t = threading.Thread(target=self.tps_timer)
        t.start()

Mod()
