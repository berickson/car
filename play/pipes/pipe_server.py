''' server '''
import os
import select
import common

def run():
    ''' server '''
    if not os.path.exists(common.pipe_name):
        os.mkfifo(common.pipe_name)
    poller = select.poll()

    pipe_fd = os.open(common.pipe_name, os.O_RDWR) # only needs read, but that blocks
    pipe_file = os.fdopen(pipe_fd)
    poller.register(pipe_fd, select.POLLIN)
    while True:
        timeout_ms = 2000
        poll_result = poller.poll(timeout_ms)
        if len(poll_result) == 0:
            print('nothing to read')
            continue
        print(pipe_file.readline(), end='')

if __name__ == '__main__':
    run()
