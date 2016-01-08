import os
# https://pyttsx.readthedocs.org/en/latest/engine.html#the-engine-factory
os.system('espeak -ven-us "hello, world!"')

def abc():
  import pyttsx
  engine = pyttsx.init('espeak')


  engine.setProperty('voice', 'english-us')
  engine.setProperty('rate', 150)
  engine.setProperty('gender','female')

  engine.say("Command mode enabled")
  engine.say("Level set to 123")
  engine.say("launching in 3, 2, 1, mark")
  engine.runAndWait()
  raw_input('press enter to continue')
