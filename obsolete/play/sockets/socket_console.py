''' example socket client '''
import sys
import socket
import time
import socket_common as c

def run(args):
    ''' run client '''
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.connect(('localhost', 5571))
    while True:
        command = input(">")
        connection.send((str(command)+"\x00").encode())
        recv_string = connection.recv(1000).decode("utf-8")
        print(recv_string)
    connection.close()

if __name__ == '__main__':
    run(sys.argv)
