import sys 

class C:
  def __init__(self):
    self.x = 3
  def bad(self,y):
    z = 7
    raise Exception("this is the exception text")

def level_2(b):
  d = 5
  c = C()
  c.bad('hello')

def level_1(a):
  b = 3
  level_2(b)

def main():
  try:
    level_1(3)
  except:
    exc_type, exc_value, tb = sys.exc_info()
    print str(tb.tb_lineno), 'exception:',exc_type,exc_value 
    if tb is not None:
      curr = tb.tb_next
      while curr is not None:
        frame = curr.tb_frame
        print "%s:%s %s()" % (frame.f_code.co_filename, frame.f_lineno, frame.f_code.co_name)
        print '  ',frame.f_locals
        curr = curr.tb_next
      del tb # required to keep from getting a memory leak



if __name__ == "__main__": main()
