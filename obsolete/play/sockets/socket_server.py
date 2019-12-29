''' socket server example '''
import socket
import select
import threading
import socket_common as c

def process_messages(client_socket):
    ''' communicate with one client '''
    print('processing messages')
    while True:
        timeout_ms = 1
        ready_to_read, _, errors = \
            select.select(
                [client_socket],
                [],
                [client_socket],
                timeout_ms)
        if len(errors):
            print('done with client')
            break

        if len(ready_to_read):
            recv_string = client_socket.recv(200).decode("utf-8")
            if len(recv_string) == 0:
                print('done with client, nothing to read')
                break
            print(recv_string, end="")

def run():
    ''' run server '''
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.bind(('localhost', c.port))
    connection.listen(5)
    print('waiting for connections')

    while True:
        (client_socket, address) = connection.accept()
        print('connected to', address)
        thread = threading.Thread(target=process_messages, args=[client_socket])
        thread.start()

if __name__ == '__main__':
    run()
