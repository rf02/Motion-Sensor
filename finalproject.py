import socket, json, asyncio
from asyncio import AbstractEventLoop
import pandas as pd

database = []
plots = []
async def echo(connection: socket, loop: AbstractEventLoop) -> None:
    while data := await loop.sock_recv(connection, 1024):
        await loop.sock_sendall(connection, data)
        handle_msg(data.decode('utf-8').strip())


async def listen_for_connection(server_socket: socket, loop: AbstractEventLoop):
    while True:
        connection, address = await loop.sock_accept(server_socket)
        connection.setblocking(False)
        print(f"Got a connection from {address}")
        asyncio.create_task(echo(connection, loop))


async def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_address = ("192.168.237.106", 1234)

    server_socket.setblocking(False)
    server_socket.bind(server_address)
    server_socket.listen()

    await listen_for_connection(server_socket, asyncio.get_event_loop())


def handle_msg(msg):
    if msg.startswith("{"):
        curr_data = json.loads(msg)
        database.append(curr_data)

    print(msg)


try:
    asyncio.run(main())
finally:
    df = pd.DataFrame.from_records(database)
    df.to_csv("output.csv")
    exit(1)
