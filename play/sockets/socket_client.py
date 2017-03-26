''' example socket client '''
import sys
import socket
import time
import socket_common as c

def run(args):
    ''' run client '''
    print('len(args)', len(args))
    per_second = 50
    total_to_send = 1000
    if len(args) >= 2:
        per_second = int(args[1])
    if len(args) >= 2:
        total_to_send = int(args[2])
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.connect(('localhost', c.port))
    count = 0
    sleep_time = 1. / per_second
    while count < total_to_send:
        count += 1
        #print("sending "+str(count))
        connection.send((str(count)+"\n").encode())
        time.sleep(sleep_time)
    connection.close()

if __name__ == '__main__':
    run(sys.argv)
