import curses

class FakeLcd:
  def __init__(self,h=2,w=16,x=10,y=10):
    self.h=h
    self.w=w
    self.x=10
    self.y=10
    
    self.stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak() # read keys instantly
    self.win = curses.newwin(self.h+2,self.w+2,self.y,self.x)
    self.stdscr.keypad(1)
    curses.curs_set(0)
    self.win.addstr(0,0,"hello")
    self.win.box()
    self.stdscr.refresh()
    self.win.refresh()
    self.stdscr.nodelay(1)
    
  # returns a curses key press as an LCD.* constant.  
  # Returns None if no key pressed.
  def getch(self):
    c = self.stdscr.getch()
    if c == curses.KEY_UP:
      return LCD.UP
    if c == curses.KEY_DOWN:
      return LCD.DOWN
    if c == curses.KEY_RIGHT:
      return LCD.RIGHT
    if c == curses.KEY_LEFT:
      return LCD.LEFT
    if c ==  ord(' '):
      return LCD.SELECT
    return None

  def __del__(self):
    curses.nocbreak()
    self.stdscr.keypad(0)
    curses.echo()
    curses.endwin()
    curses.curs_set(1)
  
  def display_text(self,s):
    lines = s.split('\n')
    for i in range(self.h):
      if len(lines)<i : break
      self.win.addstr(i+1,1, lines[i])
    self.win.refresh()

