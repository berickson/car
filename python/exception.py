import sys 


def exception_text():
  l = list()
  exc_type, exc_value, tb = sys.exc_info()
  
  l = list()
  while tb is not None:
    f = tb.tb_frame
    l.append( " {}:{} {}() {}".format(
      f.f_code.co_filename, 
      f.f_lineno, 
      f.f_code.co_name,
      f.f_locals) )
    tb = tb.tb_next
  l.reverse()

  s = 'Exception {} {} at:'.format(
    exc_type,
    exc_value)
 
  return "\n".join([s]+l)
