import logging
import xgui
import pytest

logging.basicConfig(level=logging.DEBUG)
message_log = logging.getLogger('window_messages')


def test_create_window(caplog):
    caplog.set_level(logging.WARNING)

    class TestGui(xgui.Gui):
        def callback(self, uMsg):
            if not super().callback(uMsg):
                assert "returning" in caplog.text
                return
            self.destroy_window()

    gui = TestGui("Test Window")
    gui.create_window()
    assert gui
