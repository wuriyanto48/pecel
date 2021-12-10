import socket

def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    host = '0.0.0.0'
    port = 6009
    client.connect((host, port))

    # receive initial message
    response_connected = client.recv(1024)
    print(response_connected)

    # set data
    client.send('SET 1 wury\r\n'.encode())
    response_set = client.recv(1024)
    print(response_set)

    client.send('GET 1\r\n'.encode())
    response_get = client.recv(1024)
    print(response_get)

    # exit with data
    # client.send('EXIT\r\n'.encode())
    # response_exit = client.recv(1024)
    # print(response_exit)

    # manual exit
    client.shutdown(socket.SHUT_RDWR)
    client.close()

if __name__ == '__main__':
    main()