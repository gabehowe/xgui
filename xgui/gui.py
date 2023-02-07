import enum
from ctypes import *
import cext
import logging
import uuid


class WM(enum.IntEnum):
    NULL = 0x0
    CREATE = 0x1
    DESTROY = 0x2
    MOVE = 0x3
    SIZE = 0x5
    ACTIVATE = 0x6
    SETFOCUS = 0x7
    KILLFOCUS = 0x8
    ENABLE = 0xA
    SETREDRAW = 0xB
    SETTEXT = 0xC
    GETTEXT = 0xD
    GETTEXTLENGTH = 0xE
    PAINT = 0xF
    CLOSE = 0x10


class Gui:
    def __init__(self, window_name, window_class=f"XGuiWindow-{uuid.uuid4()}", x=100, y=100, width=400, height=400):
        self.window_name = window_name
        self.window_class = window_class
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.open = False

    def create_window(self):
        """
        Creates the window.
        """
        cext.create_window(self.window_class, self.window_name, self.x, self.y, self.width, self.height, self.callback,
                           self)
        self.open = True

    def destroy_window(self):
        self.open = False
        cext.destroy_window(self.window_class, self.window_name)

    def callback(self, uMsg):
        """
        WindowProc callback.
        :param uMsg:
        :type uMsg: int
        """
        if not self.open:
            return False
