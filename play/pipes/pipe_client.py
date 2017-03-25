''' client '''
import os
import time
import common

def run():
    ''' server '''
    if not os.path.exists(common.pipe_name):
        os.mkfifo(common.pipe_name)

    pipe_file = open(common.pipe_name, "w")
    count = 0
    while True:
        count += 1
        pipe_file.write("this is line number " + str(count) + "\n")
        pipe_file.flush()
        time.sleep(1)

if __name__ == '__main__':
    run()
