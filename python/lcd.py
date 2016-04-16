import Adafruit_CharLCD as LCD

class Lcd(LCD.Adafruit_CharLCDPlate):
  def __init__(self, rows=2, columns=16):
    self.row_count = rows
    self.column_count = columns
    self.current_text = [' '*columns for r in range(rows)]
    super(Lcd, self).__init__()
    
  def display_text(self, s):
   
    line_count = self.row_count
    column_count = self.column_count
    # make input into two lines
    # of 16 characters each
    lines = s.split('\n')
    while len(lines)<line_count:
      lines.append("")
    for i in range(line_count):
      lines[i] = lines[i].ljust(column_count)[:column_count] # force exact width
      
    for r in range(line_count):
      for c in range(column_count):
        if self.current_text[r][c] <> lines[r][c]:
          self.set_cursor(c,r)
          self.write8(ord(lines[r][c]), True)
    self.current_text = lines

